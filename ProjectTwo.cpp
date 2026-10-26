// Brandon Merrell
// CS-300: Project Two - Advising Assistance Program
// File: ProjectTwo.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cctype>

using namespace std;

// Default CSV path used when no command-line argument is given
const string DEFAULT_FILE_PATH = "ABCU_Courses.csv";

// The sample output lists prerequisite course numbers only.
// Set to true to also show each prerequisite's title.
const bool SHOW_PREREQUISITE_TITLES = false;

struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

struct TreeNode {
    Course course;
    TreeNode* leftChild;
    TreeNode* rightChild;

    TreeNode(const Course& aCourse) {
        course = aCourse;
        leftChild = nullptr;
        rightChild = nullptr;
    }
};

// Helper: trim spaces, tabs, newlines, and Windows carriage returns
string trim(const string& str) {
    string result = str;
    result.erase(remove(result.begin(), result.end(), '\r'), result.end());

    size_t first = result.find_first_not_of(" \t\n");
    if (first == string::npos) return "";
    size_t last = result.find_last_not_of(" \t\n");
    return result.substr(first, last - first + 1);
}

// Helper: uppercase a string so lookups are not case sensitive
string toUpper(string str) {
    transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return str;
}

// addCourse(course): smaller courseNumber goes left, larger goes right.
TreeNode* addCourse(TreeNode* node, const Course& course, bool& added) {
    if (node == nullptr) {
        added = true;
        return new TreeNode(course);
    }

    if (course.courseNumber < node->course.courseNumber) {
        node->leftChild = addCourse(node->leftChild, course, added);
    }
    else if (course.courseNumber > node->course.courseNumber) {
        node->rightChild = addCourse(node->rightChild, course, added);
    }
    else {
        cout << "Warning: Duplicate course " << course.courseNumber << " ignored." << endl;
        added = false;
    }

    return node;
}

// searchCourse(courseNumber): move left or right until found or NULL
TreeNode* searchCourse(TreeNode* node, const string& courseNumber) {
    while (node != nullptr && node->course.courseNumber != courseNumber) {
        if (courseNumber < node->course.courseNumber) {
            node = node->leftChild;
        }
        else {
            node = node->rightChild;
        }
    }
    return node;
}

// printCourses(node): print left subtree, current Course, then right subtree
void printCourses(TreeNode* node) {
    if (node != nullptr) {
        printCourses(node->leftChild);
        cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
        printCourses(node->rightChild);
    }
}

// printCourseDetails(course): number, title, then prerequisites
void printCourseDetails(TreeNode* root, const Course& course) {
    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";
    for (size_t i = 0; i < course.prerequisites.size(); ++i) {
        const string& prereqCode = course.prerequisites[i];
        TreeNode* prereqNode = searchCourse(root, prereqCode);

        cout << prereqCode;
        if (SHOW_PREREQUISITE_TITLES && prereqNode != nullptr) {
            cout << " (" << prereqNode->course.courseTitle << ")";
        }
        if (i + 1 < course.prerequisites.size()) {
            cout << ", ";
        }
    }
    cout << endl;
}

// Search for a course and print its details, or 'Course not found'
void findCourse(TreeNode* root, const string& userSearch) {
    string searchKey = toUpper(trim(userSearch));
    TreeNode* match = searchCourse(root, searchKey);

    if (match == nullptr) {
        cout << "Course not found" << endl;
    }
    else {
        printCourseDetails(root, match->course);
    }
}

// Free every node in the tree
void clearTree(TreeNode*& node) {
    if (node == nullptr) return;
    clearTree(node->leftChild);
    clearTree(node->rightChild);
    delete node;
    node = nullptr;
}

// loadCourses(filePath, structure): parse + validate everything first
bool loadCourses(const string& filePath, TreeNode*& root) {
    // Open the CSV file; if it cannot open, report an error and return failure
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filePath << endl;
        return false;
    }

    // Read each nonblank line, trim, split by comma, and save the line
    vector<vector<string>> savedLines;
    set<string> courseNumbers;   // collected before validating prerequisites
    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        ++lineNumber;

        // Skip a UTF-8 byte-order mark if Excel added one
        if (lineNumber == 1 && line.size() >= 3 && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line = line.substr(3);
        }

        line = trim(line);
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }

        // Fewer than two fields or missing number/title: format error, fail
        if (tokens.size() < 2 || tokens[0].empty() || tokens[1].empty()) {
            cout << "Format error on line " << lineNumber << " of " << filePath << endl;
            file.close();
            return false;
        }

        courseNumbers.insert(toUpper(tokens[0]));
        savedLines.push_back(tokens);
    }
    file.close();

    // For each saved line, create one Course and validate its prerequisites
    vector<Course> courses;
    for (const vector<string>& tokens : savedLines) {
        Course course;
        course.courseNumber = toUpper(tokens[0]);
        course.courseTitle = tokens[1];

        for (size_t i = 2; i < tokens.size(); ++i) {
            if (tokens[i].empty()) continue;

            string prereq = toUpper(tokens[i]);
            if (courseNumbers.find(prereq) == courseNumbers.end()) {
                cout << "Error: Prerequisite " << prereq << " for course "
                    << course.courseNumber << " does not exist." << endl;
                return false;
            }
            course.prerequisites.push_back(prereq);
        }
        courses.push_back(course);
    }

    // All lines validated: start from an empty tree, then addCourse for each
    clearTree(root);
    int count = 0;
    for (const Course& course : courses) {
        bool added = false;
        root = addCourse(root, course, added);
        if (added) ++count;
    }

    cout << "Loaded " << count << " courses successfully." << endl;
    return true;
}

// Main Function() // Menu Loop
int main(int argc, char* argv[]) {
    TreeNode* root = nullptr;
    bool loaded = false;
    string menuChoice = "";
    string userSearch = "";

    string filePath;
    if (argc > 1) {
        filePath = argv[1];
    }
    else {
        filePath = DEFAULT_FILE_PATH;
    }

    cout << "Welcome to the course planner." << endl;

    while (menuChoice != "9") {
        cout << "\n  1. Load Courses" << endl;
        cout << "  2. Display Courses" << endl;
        cout << "  3. Find Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "\nWhat would you like to do? ";

        string rawInput;
        if (!getline(cin, rawInput)) break;
        menuChoice = trim(rawInput);

        if (menuChoice == "1") {
            // Prompt for the file; pressing Enter alone uses the command-line/default path
            cout << "Enter the file name containing course data: ";
            string fileInput;
            if (!getline(cin, fileInput)) break;
            fileInput = trim(fileInput);

            // Strip surrounding quotes (Windows "Copy as path" adds them)
            if (fileInput.size() >= 2 && fileInput.front() == '"' && fileInput.back() == '"') {
                fileInput = fileInput.substr(1, fileInput.size() - 2);
            }
            if (!fileInput.empty()) {
                filePath = fileInput;
            }

            if (loadCourses(filePath, root)) {
                loaded = true;
            }
            else {
                loaded = false;
                cout << "Courses could not be loaded." << endl;
            }
        }
        else if (menuChoice == "2") {
            if (!loaded) {
                cout << "Load courses first" << endl;
            }
            else {
                cout << "\nHere is a sample schedule:\n" << endl;
                printCourses(root);
            }
        }
        else if (menuChoice == "3") {
            if (!loaded) {
                cout << "Load courses first" << endl;
            }
            else {
                cout << "What course do you want to know about? ";
                if (getline(cin, userSearch)) {
                    findCourse(root, userSearch);
                }
            }
        }
        else if (menuChoice == "9") {
            cout << "Good bye" << endl;
        }
        else {
            cout << menuChoice << " is not a valid option." << endl;
        }
    }

    clearTree(root);
    return 0;
}
