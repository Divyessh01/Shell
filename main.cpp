#include <iostream>
#include <string>
#include <cstring>
#include <sys/utsname.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <queue>
#include <sys/stat.h>
#include <sys/types.h>
#include <iomanip>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <wait.h>
using namespace std;

// read input from user | store to input
string getuserinput(){
    string input;
    getline(cin, input);
    return input;
}

// split input into lines by ; | store lines to queue lines
void split_input(queue<string>& lines, string input){
    char* temp = (char*)malloc((input.size()+1)* sizeof(char));
    std::strcpy(temp, input.c_str());

    char* line = strtok(temp, ";");
    while(line != nullptr){
        lines.push(string(line));
        line = strtok(nullptr, ";");
    }
    
}

// split lines into words | store words to vector words
void tokenize(vector<string>& words, string line){
    char* temp = (char*)malloc((line.size()+1)* sizeof(char));
    std::strcpy(temp, line.c_str());

    char* word = strtok(temp, " \t");
    while(word != nullptr){
        words.push_back(word);
        word = strtok(nullptr, " \t");
    }
    
    free(temp);
}

void print_vector(vector<string>& words){
    cout << "============tokens===========" << endl;

    for(auto word: words){
        cout << word << endl;
    }

    cout << "=============================" << endl;
}


void fun_echo(vector<string> words){

    for(int i = 1; i < words.size(); i++){
        cout << words[i] + " ";
    }
    cout << endl;
}

// function pwd
void fun_pwd(vector<string> words){
    if(words.size() > 1){
        errno = E2BIG;
        perror("Error");
        return;
    }
    char* temp = (char*)malloc(sizeof(char) * 256);

    string pwd = getcwd(temp, 256);
    cout << pwd << endl;
}

// function cd
void fun_cd(vector<string> words, string root,string& curr_dir,string& prev_dir){
    string prev;
    if(prev_dir[0] == '~'){
        if(prev_dir.length() == 1)
            prev = root;
        else prev = root + prev_dir.substr(1);
    }
    else prev = prev_dir;

    if(words.size() > 2){
        perror("Error:");
        return;
    } else if(words.size() == 1){
        chdir(root.c_str());
        prev_dir = curr_dir;
        curr_dir = "~";
    } else {
        if(words[1] == "/"){
            errno = EINVAL;
            perror("Error:");
            return;
        }
        if(words[1] == "~"){
            chdir(root.c_str());
            prev_dir = curr_dir;
            curr_dir = "~";
        }
        else{
            if(words[1] == "-"){
                if(chdir(prev.c_str()) == 0){
                    prev_dir = curr_dir;
                }else {
                    perror("Error");
                    return;
                }
            }
            else{
                if(chdir(words[1].c_str()) == 0){
                    prev_dir = curr_dir;
                }else {
                    perror("Error");
                    return;
                }
            }
            char* temp = (char*)malloc(sizeof(char) * 256);
            string path = getcwd(temp, 256);

            if((path.find(root, 0)) == 0) {
                curr_dir = "~" + path.substr(root.length());
            } else{
                curr_dir = path;
            }
        } 
    }
}


// get permission information for ls -l
string get_permission_info(struct stat perm){
    string res = "";\
    res += S_ISDIR(perm.st_mode)?"d":"-";
    res += (perm.st_mode & S_IRUSR)?"r":"-";
    res += (perm.st_mode & S_IWUSR)?"w":"-";
    res += (perm.st_mode & S_IXUSR)?"x":"-";
    res += (perm.st_mode & S_IRGRP)?"r":"-";
    res += (perm.st_mode & S_IWGRP)?"w":"-";
    res += (perm.st_mode & S_IXGRP)?"x":"-";
    res += (perm.st_mode & S_IROTH)?"r":"-";
    res += (perm.st_mode & S_IWOTH)?"w":"-";
    res += (perm.st_mode & S_IXOTH)?"x":"-";
    return res;
}

// call ls -l for each directory(as dirpath) inside the path
void fun_ls_l(string dirpath){

    struct stat info;
    if (stat(dirpath.c_str(), &info) != 0){
        perror("Error:");
        return;
    }else {

        string permissions = get_permission_info(info);
        cout << permissions << " ";
        cout << setw(3) << right << info.st_nlink << " ";

        cout << getpwuid(info.st_uid)->pw_name << " ";
        cout << getgrgid(info.st_gid)->gr_name << " ";
        cout << setw(6) << right << info.st_size << " ";

        tm* timeinfo = localtime(&info.st_mtime);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%b %d %H:%M", timeinfo);
        cout << buffer << " ";
    }

}

// function ls
void fun_ls(vector<string> words, string root){
    string path = root;
    bool is_a = false;
    bool is_l = false;
    bool no_input = true;

    for(int i = 1; i < words.size(); i++){

        if(words[i][0] == '-'){
            for(int ii = 1; ii < words[i].size(); ii++){
                if(words[i][ii] == 'a')
                    is_a = true;
                else if(words[i][ii] == 'l')
                    is_l = true;
                else{
                    errno = EINVAL;
                    perror("Error:");
                    return;
                }
            }
        }else {
            no_input = false;
            if(words[i][0] != '/')
                path = root + '/' + words[i];
            else
                path = root + words[i];

            DIR* dir = opendir(path.c_str());
            if (dir) {
                cout << path  + ": " << endl;
                struct dirent* entry;
                while ((entry = readdir(dir)) != NULL) {
                    if(!is_a){
                        if(entry->d_name[0] == '.') continue;
                    }

                    if(is_l){
                        fun_ls_l(path+"/"+entry->d_name);
                        cout << entry->d_name << endl;
                    } else {
                        cout << entry->d_name << endl;
                    }
                }
                closedir(dir);
            }else {
                perror("Error:");
                return;
            }
        }
    }

    if(no_input){
        DIR* dir = opendir(path.c_str());
        if (dir) {
            cout << path  + ": " << endl;
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) {
                if(!is_a){
                    if(entry->d_name[0] == '.') continue;
                }

                if(is_l){
                    fun_ls_l(path+"/"+entry->d_name);
                    cout << entry->d_name << endl;
                } else {
                    cout << entry->d_name << endl;
                }
            }
            closedir(dir);
        }else {
            perror("Error:");
            return;
        }
    }        
}

// function to create background process and return its pid
void fun_bg(vector<string> words){

    words.pop_back();
    const char* temp = (char*)malloc(sizeof(char) * 32);
    temp = words[0].c_str();

    char** other_args = (char**)malloc((words.size() + 1) * sizeof(char*));
    for (size_t i = 0; i < words.size(); ++i) {
        other_args[i] = strdup(words[i].c_str());
    } 
    other_args[words.size()] = NULL;

    pid_t pid;
    pid = fork();
    if(pid < 0){
        perror("Error:");
        return;
    }

    if (pid == 0) { 
        if (execvp(temp, (char* const*)other_args) == -1) {
            perror("Error:");
            EXIT_FAILURE;
        }
    } else{
        cout << pid << endl;
    }
}


// default call for execvp
void fun_default(vector<string> words){
    const char* temp = (char*)malloc(sizeof(char) * 32);
    temp = words[0].c_str();

    char** other_args = (char**)malloc((words.size() + 1) * sizeof(char*));
    for (size_t i = 0; i < words.size(); ++i) {
        other_args[i] = strdup(words[i].c_str());
    } 
    other_args[words.size()] = NULL;

    pid_t pid = fork();

    if (pid == -1) {
        perror("Error:");
        return;
    }

    if (pid == 0) { 
        if (execvp(temp, (char* const*)other_args) == -1) {
            errno = ENOEXEC;
            perror("Error: Invalid command");
            EXIT_FAILURE;
        }
    }else {
        int status;
        waitpid(pid, &status, 0);
    }
}

int main(){

    char* temp = (char*)malloc(1024 * sizeof(char));

    const string username = getlogin();
    gethostname(temp, 100);
    const string hostname(temp);
    string current_dir = "~";

    const string root = getcwd(temp, 256);
    string prev_dir = root;

    queue<string> lines;
    vector<string> words;
    while(true){
        cout << username << "@" << hostname << ":" << current_dir << ">";
        string input = getuserinput();

        split_input(lines, input);

        bool is_exit = false;
        while(!lines.empty()){
            string line = lines.front();
            lines.pop();

            tokenize(words, line);
            //print_vector(words);

            if(words.size() == 0) continue;

            if(words[0] == "exit" or words[0] == "quit") {
                is_exit = true;
                break;
            } else if(words[0] == "echo") {
                fun_echo(words);
            }else if(words[0] == "pwd") {
                fun_pwd(words);
            }else if(words[0] == "cd") {
                fun_cd(words, root, current_dir, prev_dir);
            }else if(words[0] == "ls") {
                fun_ls(words, root);
            }else if(words[words.size()-1] == "&") {
                fun_bg(words);
            } else {
                fun_default(words);
            }

            words.clear();
        }

        if(is_exit) break;

    }

    return 0;
}