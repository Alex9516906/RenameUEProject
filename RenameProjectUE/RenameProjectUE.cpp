#include <fstream>
#include <iostream>
#include <string>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <string>
#include <filesystem>

namespace fs = std::experimental::filesystem;

static std::string PathToSourceFolder = "D:/ForTest/Source";
static std::string OldPrefix = "Survival";
static std::string OldNameProject = "SurvivalGame";
static std::string oldTextAPI = "SURVIVALGAME_API";
static std::string NewPrefix = "NewSurvival";
static std::string NewNameProject = "NewSurvivalGame";
static std::string newTextAPI = "NEWSURVIVALGAME_API";

std::string GetPathToProjectDir()
{
    PathToSourceFolder;
    fs::path path(PathToSourceFolder);
    auto parentPath = path.parent_path();

    return parentPath.string();
}

void ChangeNameInUproject(std::string mOldName, std::string mNewName)
{
    std::string UprojectPath = GetPathToProjectDir() + "/" + OldNameProject + ".uproject";
    std::ifstream file(UprojectPath);
    if (!file)
    {
        std::cerr << "Error opening file: " << UprojectPath << std::endl;
        return;
    }
    std::cerr << UprojectPath << " open" << std::endl;

    std::string fileContent((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    // Replace the given line
    size_t pos = 0;
    size_t posNew = 0;

    while ((pos = fileContent.find(mOldName, pos)) != std::string::npos)
    {
        bool InConstructorHelper = false;

        if (pos > 80)
        {
            posNew = fileContent.find("TEXT", pos - 80);
            if (posNew != std::string::npos && posNew < pos)
            {
                InConstructorHelper = true;
            }
        }
        if (!InConstructorHelper)
        {
            fileContent.replace(pos, mOldName.length(), mNewName);
            pos += mNewName.length();
        }
        else
        {
            pos += mOldName.length();
        }
    }
    // Opening a file for writing
    std::ofstream outFile(UprojectPath);
    if (!outFile)
    {
        std::cerr << "Error opening file: " << UprojectPath << std::endl;
        return;
    }
    std::cerr << UprojectPath << " done" << std::endl;
    // Write the modified file to disk
    outFile << fileContent;
}

void ChangeAllNameInConfig(std::string mOldName, std::string mNewName, std::string Path)
{
    fs::recursive_directory_iterator;
    // Loop through files in a folder and subfolders
    for (const auto& entry : fs::recursive_directory_iterator(Path))
    {
        // Check if this is a text file
        if (entry.path().extension() == ".cpp" || entry.path().extension() == ".ini"|| entry.path().extension() == ".h" || entry.path().extension() == ".cs" ||  entry.path().extension() == ".h")
        {
            // Opening a file for reading
            std::ifstream file(entry.path());
            if (!file)
            {
                std::cerr << "Error opening file: " << entry.path() << std::endl;
                continue;
            }
            std::cerr << entry.path() << " open" << std::endl;
           
            std::string fileContent((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

            // Replace the given line
            size_t pos = 0;
            size_t posNew = 0;
          
            while ((pos = fileContent.find(mOldName, pos)) != std::string::npos)
            {
                bool InConstructorHelper = false;

                if (pos > 80)
                {
                    posNew = fileContent.find("TEXT(\"/Game", pos - 80);
                    if (posNew  != std::string::npos && posNew < pos)
                    {
                        InConstructorHelper = true;
                    }
                }
                if (!InConstructorHelper)
                {
                    fileContent.replace(pos, mOldName.length(), mNewName);
                    pos += mNewName.length();
                }
                else
                {
                    pos += mOldName.length();
                }
            }
            // Opening a file for writing
            std::ofstream outFile(entry.path());
            if (!outFile)
            {
                std::cerr << "Error opening file: " << entry.path() << std::endl;
                continue;
            }
            std::cerr << entry.path() << " done" << std::endl;
            // Write the modified file to disk
            outFile << fileContent;
        }
    }
    ChangeNameInUproject(mOldName, mNewName);
}



void ChangeNameInAllFilesInSource(std::string mOldName, std::string mNewName)
{
    ChangeAllNameInConfig(mOldName, mNewName, PathToSourceFolder);
   ChangeAllNameInConfig(mOldName, mNewName, GetPathToProjectDir()+"/Config");
}

void RenameAllFiles(std::string oldText, std::string newText)
{
    fs::recursive_directory_iterator;
    // Open a text file to write the results
    std::ofstream output(GetPathToProjectDir()+ "/" + "rename_results.txt");
    if (!output.is_open())
    {
        std::cerr << "Error: Could not open rename_results.txt for writing" << std::endl;
        return;
    }

    // Loop through files and folders in a folder and its subfolders
    for (const auto& entry : fs::recursive_directory_iterator(PathToSourceFolder))
    {
        
        std::string oldName = entry.path().filename().string();
        std::string OldNameToRedirect = entry.path().stem().string();
        if (oldName.find(oldText) == std::string::npos)
        {
            // Line not found, skip this file
            continue;
        }

        // Generating a new filename
        std::string newName = oldName;
        newName.replace(newName.find(oldText), oldText.length(), newText);
        
        try
        {
            fs::rename(entry, entry.path().parent_path() / newName);
        }
        catch (const fs::filesystem_error& e)
        {
            std::cerr << "Error: " << e.what() << entry.path().parent_path().string() <<" "<< newName << std::endl;
        }
        
        if (newName.find(".cpp") == std::string::npos)
        {
            if (newName.find(".h")!= std::string::npos)
            {
                newName.replace(newName.find(".h"), 2, "");
                output << "+ClassRedirects=(OldName=\"/Script/SurvivalGame." << OldNameToRedirect << "\", NewName=\"/Script/LostRegion." << newName << "\")" << std::endl;
            }
        }
    }

    output.close();
}

void RenameProjectName()
{
    std::string folder = GetPathToProjectDir();

    // Looping files in a folder
    for (const auto& entry : fs::directory_iterator(folder))
    {

        // Checking that the file has the extension ".uproject"
        if (entry.path().extension() == ".uproject")
        {
            std::string newName = entry.path().filename().string();
            if (newName.find(OldNameProject) == std::string::npos)
            {
                return;
            }
            newName.replace(newName.find(OldNameProject), OldNameProject.length(), NewNameProject);
            try
            {
                fs::rename(entry, entry.path().parent_path() / newName);
            }
            catch (const std::exception&)
            {
                std::cerr << "Error: file .uproject not found" << std::endl;
            }
            
        }
    }
}

void DeleteStagingFile()
{
    std::vector<std::string> folders = { "Intermediate", "Binaries","Saved","DerivedDataCache", OldNameProject + ".sln", ".vs", ".idea"};
    for (auto folder : folders)
    {
        try
        {
            fs::remove_all(GetPathToProjectDir() + "/" + folder);
            std::cout << "Folder deleted successfully" << std::endl;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    } 
}

int main(int argc, char* argv[])
{
    fs::recursive_directory_iterator;

    ChangeNameInAllFilesInSource(oldTextAPI, newTextAPI);
    ChangeNameInAllFilesInSource(OldNameProject, NewNameProject);
    ChangeNameInAllFilesInSource(OldPrefix,NewPrefix);

    RenameAllFiles(OldNameProject, NewNameProject);
    RenameAllFiles(OldNameProject, NewNameProject);
    RenameAllFiles(OldPrefix, NewPrefix);

    DeleteStagingFile();

    RenameProjectName();

   // AddToGameEngineRedirect(); TODO
    return 0;
}


