#include <cstring>
#include <SD.h>
#include "MySD.h"


//前提: ルートディレクトリ直下にサブフォルダが存在しないこと（通常ファイルのみを想定）
bool MySD::init(int& fileCount, std::vector<char>& fileNames) {

    File root = SD.open("/");
    if (!root) {
        return false;
    }

    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) { // フォルダは無視してファイルだけ
            const char* fName = file.name();
            if (strncmp(fName, "_intro_", 7) != 0) {//この判定の時には'/'はないと仮定．イントロは表示させたくないので除く
                fileNames.insert(fileNames.end(), fName, fName + strlen(fName) + 1);
                fileCount++;
            }   
        }
        file.close();
        file = root.openNextFile();
    }
    root.close();
    return true;
}

const char* MySD::extract_FileAdress(const std::vector<char>& fileNames, int cursorIndex) {
    int count{0};//ほしいファイルの先頭までの文字数カウント
    for (int i = 0; i < cursorIndex; ++i) {
        count += strlen(&fileNames[count]) + 1;
    }
    return &fileNames[count];
}

//拡張子判別
FileType MySD::judge_FileType(const char* ext) {
    if (ext == nullptr) {
        return FileType::UNKNOWN;
    }

    for (const auto& pair : extMap) {
        if (strcmp(ext, pair.key) == 0) {
            return pair.value;
        }
    }
    return FileType::UNKNOWN;
}

//拡張子抽出
FileType MySD::get_FileType(const char* fileHead) {
    const char* ext = strrchr(fileHead, '.');
    return judge_FileType(ext);
}

File MySD::getFile(const char* path)
{
    File file = SD.open(path);
    return file;
}