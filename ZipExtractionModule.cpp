/*
 * The Sleuth Kit
 *
 * Contact: Brian Carrier [carrier <at> sleuthkit [dot] org]
 * Copyright (c) 2010-2011 Basis Technology Corporation. All Rights
 * reserved.
 *
 * This software is distributed under the Common Public License 1.0
 */

/**
 * \file ZipExtractionModule.cpp
 * Contains the implementation for the Zip extraction file analysis module.
 * This module extracts zip file content and creates entries in the database 
 * for the extracted files. 
 */

// Framework includes
#include "TskModuleDev.h"

// System includes
#ifdef TSK_WIN32
#include <windows.h>
#else
#error "Only Windows is currently supported"
#endif

#include <sstream>
#include <iostream>
#include <fstream>

// Poco includes
#include "Poco/Path.h"
#include "Poco/Zip/ZipArchive.h"
#include "Poco/Zip/ZipStream.h"
#include "Poco/Zip/Decompress.h"

extern "C" 
{
    /**
     * Get the file id corresponding to the last directory on the given path.
     * If elements along the path have not been seen before, create new entries
     * for those elements in the database and in the directory map (3rd parameter). 
	 * The parent id for top level directories will be the file id of the zip file.
     */
    uint64_t getParentIdForPath(Poco::Path& path, const uint64_t fileId, std::string parentPath, std::map<std::string, uint64_t>& directoryMap)
    {
        // If the path references a file, make it refer to to its parent instead
        if (path.isFile())
            path = path.makeParent();

        // Initialize parent id to be the file id of the zip file.
        uint64_t parentId = fileId;

        // Iterate over every element of the path checking to see if we 
        // already have an entry in the database and in the directory map.
        Poco::Path tempPath;
        TskImgDB& imgDB = TskServices::Instance().getImgDB();
        std::map<std::string, uint64_t>::const_iterator pos;
        for (int i = 0; i < path.depth(); i++)
        {
            // Build up a temporary path that only contains the path
            // elements seen so far. This temporary path will be used
            // below to add the full path to the map.
            tempPath.pushDirectory(path[i]);

            // Have we already seen this path?
            pos = directoryMap.find(tempPath.toString());

            if (pos == directoryMap.end())
            {
				std::string fullpath = "";
                fullpath.append(parentPath);
                fullpath.append("\\");
                fullpath.append(path.toString());

                // No entry exists for this directory so we create one.
                if (imgDB.addDerivedFileInfo(path[i], parentId,
                                             true, // isDirectory
                                             0, // uncompressed size
                                             "", // no details
                                             0, // ctime
                                             0, // crtime
                                             0, // atime
                                             0, // mtime
                                             parentId,
                                             fullpath) == -1)
                {
                    std::wstringstream msg;
                    msg << L"ZipExtractionModule - addDerivedFileInfo failed for name="
                        << path[i].c_str();
                    LOGERROR(msg.str());
                }

                // Add the full path (to this point) and new id to the map.
                directoryMap[tempPath.toString()] = parentId;

                // Update file status to indicate that it is ready for analysis.
                imgDB.updateFileStatus(parentId, TskImgDB::IMGDB_FILES_STATUS_READY_FOR_ANALYSIS);
            }
            else
            {
                parentId = pos->second;
            }
        }

        return parentId;
    }

    TskModule::Status TSK_MODULE_EXPORT initialize(std::string& args)
    {
        return TskModule::OK;
    }

    TskModule::Status TSK_MODULE_EXPORT run(TskFile * pFile)
    {
        if (pFile == NULL)
        {
            LOGERROR(L"Zip extraction module passed NULL file pointer.");
            return TskModule::FAIL;
        }

        try
        {
			// Create a map of directory names to file ids so that we can
			// associate files/directories with the correct parent.
			std::map<std::string, uint64_t> directoryMap;

            TskImgDB& imgDB = TskServices::Instance().getImgDB();

            // We need to have the file on disk so save it.
            pFile->save();

            std::ifstream input(pFile->getPath().c_str(), std::ios_base::binary);

            Poco::Zip::ZipArchive archive(input);
            Poco::Zip::ZipArchive::FileHeaders::const_iterator fh;

            uint64_t parentId = 0;

            for (fh = archive.headerBegin(); fh != archive.headerEnd(); ++fh)
            {
                Poco::Path path(fh->first);
                Poco::Path parent = path.parent();
                std::string name;

                if (path.isDirectory())
                    name = path[path.depth() - 1];
                else
                    name = path[path.depth()];

                // Determine what our parent id should be.
                if (path.depth() == 0 || path.isDirectory() && path.depth() == 1)
                    // This file or directory lives at the root so our parent id
                    // is the containing file id.
                    parentId = pFile->id();
                else
                {
                    // We are not at the root so we need to lookup the id of our
                    // parent directory
                    std::map<std::string, uint64_t>::const_iterator pos;
                    pos = directoryMap.find(parent.toString());

                    if (pos == directoryMap.end())
                    {
                        // In certain circumstances (Windows Send to zip and .docx files)
                        // there may not be entries in the zip file for directories.
                        // For these cases we create database entries for the directories
                        // so that we can accurately track parent relationships. The
                        // getParentIdForPath() method creates the database entries for the
                        // given path and returns the parentId of the last directory on the path.
                        parentId = getParentIdForPath(parent, pFile->id(), pFile->fullPath(), directoryMap);
                    }
                    else
                    {
                        parentId = pos->second;
                    }
                }

                // Store some extra details about the derived file
                std::stringstream details;
                details << "<ZIPFILE name=\"" << fh->second.getFileName()
                    << "\" compressed_size=\"" << fh->second.getCompressedSize()
                    << "\" uncompressed_size=\"" << fh->second.getUncompressedSize()
                    << "\" crc=\"" << fh->second.getCRC()
                    << "\" start_pos=\"" << fh->second.getStartPos()
                    << "\" end_pos=\"" << fh->second.getEndPos()
                    << "\" major_version=\"" << fh->second.getMajorVersionNumber()
                    << "\" minor_version=\"" << fh->second.getMinorVersionNumber() << "\""
                    << "</ZIPFILE>";

                uint64_t fileId;

                std::string fullpath = "";
                fullpath.append(pFile->fullPath());
                fullpath.append("\\");
                fullpath.append(path.toString());

                if (imgDB.addDerivedFileInfo(name,
                    parentId,
                    path.isDirectory(),
                    fh->second.getUncompressedSize(),
                    details.str(), 
                    0, // ctime
                    0, // crtime
                    0, // atime
                    static_cast<int>(fh->second.lastModifiedAt().utcTime()),
                    fileId, fullpath) == -1) 
                {
                        std::wstringstream msg;
                        msg << L"ZipExtractionModule - addDerivedFileInfo failed for name="
                            << name.c_str();
                        LOGERROR(msg.str());
                }

                TskImgDB::FILE_STATUS fileStatus = TskImgDB::IMGDB_FILES_STATUS_READY_FOR_ANALYSIS;

                if (path.isDirectory())
                    directoryMap[path.toString()] = fileId;
                else
                {
                    // Only DEFLATE and STORE compression methods are supported. The STORE method
                    // simply stores a file without compression.
                    if (fh->second.getCompressionMethod() == Poco::Zip::ZipCommon::CM_DEFLATE ||
                        fh->second.getCompressionMethod() == Poco::Zip::ZipCommon::CM_STORE)
                    {
                        // Save the file for subsequent processing.
                        Poco::Zip::ZipInputStream zipin(input, fh->second);
                        TskServices::Instance().getFileManager().addFile(fileId, zipin);

						// Schedule subsequent processing.
						TskServices::Instance().getScheduler().schedule(Scheduler::FileAnalysis, fileId, fileId);
                    }
                    else
                    {
                        std::wstringstream msg;
                        msg << L"ZipExtractionModule - Unsupported compression method for file: "
                            << name.c_str();
                        LOGWARN(msg.str());
                        
                        fileStatus = TskImgDB::IMGDB_FILES_STATUS_ANALYSIS_FAILED;
                    }
                }

                // Update file status to indicate that it is ready for analysis.
                imgDB.updateFileStatus(fileId, fileStatus);
            }
        }
        catch (Poco::IllegalStateException&)
        {
            // Poco::IllegalStateException is thrown if the file is not a valid zip file
            // so we simply skip the file.
            return TskModule::OK;
        }
        catch (Poco::AssertionViolationException&)
        {
            // Corrupt zip files are not uncommon, especially for carved files.
            std::wstringstream msg;
            msg << L"ZipExtractionModule - Encountered corrupt zip file ( " << pFile->name().c_str()
                << L")";
            LOGWARN(msg.str());

            return TskModule::FAIL;
        }
        catch (std::exception& ex)
        {
            std::wstringstream msg;
            msg << L"ZipExtractionModule - Error processing zip file ( " << pFile->name().c_str()
                << L") : " << ex.what();
            LOGERROR(msg.str());

            return TskModule::FAIL;
        }

        return TskModule::OK;
    }

    TskModule::Status TSK_MODULE_EXPORT finalize()
    {
        return TskModule::OK;
    }
}