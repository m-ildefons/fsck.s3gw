/*
 * Copyright 2023 SUSE, LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * - - -
 *
 * Orphaned Metadata Check
 * This check will iterate over all object metadata in the database and ensure
 * that the referenced objects can be found in the filesystem on disk. Should an
 * object or a version of an object not be found on disk, when the metadata
 * suggests it should be there, the fix will be to delete the metadata.
 */

#ifndef FSCK_S3GW_SRC_CHECKS_ORPHANED_METADATA_H__
#define FSCK_S3GW_SRC_CHECKS_ORPHANED_METADATA_H__

#include <filesystem>
#include <memory>

#include "checks.h"
#include "sqlite.h"

class OrphanedMetadataFix : public Fix {
 private:
  std::filesystem::path root_path;
  std::filesystem::path obj_path;  // relative to root_path

  std::string to_string() const;

 public:
  OrphanedMetadataFix(std::filesystem::path, std::filesystem::path);
  operator std::string() const { return to_string(); };
  void fix();
};

class OrphanedMetadataCheck : public Check {
 private:
  std::filesystem::path root_path;
  std::unique_ptr<Database> metadata;

 public:
  OrphanedMetadataCheck(std::filesystem::path);
  virtual ~OrphanedMetadataCheck() override;
  virtual int check() override;
};

#endif  // FSCK_S3GW_SRC_CHECKS_ORPHANED_METADATA_H__
