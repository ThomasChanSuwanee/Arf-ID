#include "sdcard.h"
// return string of the file path

static std::string csvHeader = "EPC, RSSI, Reading Time\n";
void writeEPC(std::string filename, std::string epc, std::string reading_time)
{
  // append to file
  appendFile(SD, filename.c_str(), epc.c_str());
  appendFile(SD, filename.c_str(), ",");
  appendFile(SD, filename.c_str(), reading_time.c_str());
  appendFile(SD, filename.c_str(), "\n");
}

std::string SD_setup(std::string current_date)
{
  SPI.begin(14, 39, 12, 11);
  SD.begin(11);
  if (!SD.exists("/mydir"))
  {
    createDir(SD, "/mydir");
  }
  std::vector<std::string> files = listDir(SD, "/mydir", 0);
  for (std::string file : files)
  {
    Serial.println(file.c_str());
  }

  // create new file with current date
  int count = 0;
  std::string filename = "/mydir/" + current_date + "_" + std::to_string(count) + ".txt";

  while (SD.exists(filename.c_str()))
  {
    filename = "/mydir/" + current_date + "_" + std::to_string(count) + ".txt";
    count = count + 1;
  }
  writeFile(SD, filename.c_str(), csvHeader.c_str());
  return filename; // return the full path of the new file
}

std::vector<std::string> listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  std::vector<std::string> files_list;
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return files_list;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return files_list;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      // Serial.print("  DIR : ");
      // Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      // Serial.print("  FILE: ");
      // Serial.print(file.name());
      std::string filename_string = file.name();
      files_list.push_back(filename_string);

      // Serial.print("  SIZE: ");
      // Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  return files_list;
}

void createDir(fs::FS &fs, const char *path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path))
  {
    Serial.println("Dir created");
  }
  else
  {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path))
  {
    Serial.println("Dir removed");
  }
  else
  {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
   // Serial.println("Message appended");
  }
  else
  {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    Serial.println("File renamed");
  }
  else
  {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path))
  {
    Serial.println("File deleted");
  }
  else
  {
    Serial.println("Delete failed");
  }
}

void saveDataSD(string filename, string epc, string reading_time)
{
  // append to file
  appendFile(SD, filename.c_str(), epc.c_str());
  appendFile(SD, filename.c_str(), ",");
  appendFile(SD, filename.c_str(), reading_time.c_str());
  appendFile(SD, filename.c_str(), "\n");
  Serial.println("Data saved to SD card.");
}