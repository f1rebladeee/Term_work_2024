#include "Term_work_backup.h"
#include "interpretations.h"

struct Term {
    size_t number;
    Term* next;
    map<string, map <string, string> > marks;
    vector <string>* subjects;
    Term(size_t number) {
        this->number = number;
        this->next = nullptr;
        this->subjects = &TermSubjects[number - 1];
    }
};

class LinkedList {
public:
    Term* head, * tail;

public:
    LinkedList() {
        this->head = this->tail = nullptr;
    }

    ~LinkedList() {
        while (head != nullptr) pop_front();
    }

    void pop_front() {
        if (head == nullptr) return;
        if (head == tail) {
            delete tail;
            head = tail = nullptr;
            return;
        }

        Term* term = head;
        head = term->next;
        delete term;
    }

    void push_back(short number) {
        Term* term = new Term(number);
        if (head == nullptr) head = term;
        if (tail != nullptr) tail->next = term;
        tail = term;
    }

    Term* operator[](size_t index) const {
        Term* term = head;
        unsigned short cnt = 0;
        while (term && cnt != index && term->next) {
            term = term->next;
            cnt++;
        }
        return (cnt == index) ? term : nullptr;
    }
};

///|---------------------Global variables---------------------|
set <string> recordBooks;
LinkedList Grades;
vector<Student> v_students, v_otl, v_hor, v_ud;
DWORD dwIndex = 0;
DWORD dwType = 0;
DWORD caName = 0;
LPTSTR pszName, x;
HCRYPTPROV hProv;
HCRYPTKEY hSessionKey;
///|----------------------------------------------------------|

void createGrades() {
    for (short i = 1; i <= 9; i++) Grades.push_back(i);
}

void gradesFill(vector<string> &grade_lines) {
    ifstream input_grades("grades.txt", ios_base::binary);
    short term;
    string record_book, mark;
    DWORD cnt;
    for (int i = 0; i < grade_lines.size(); i++) {
        cnt = static_cast<DWORD>(grade_lines[i].size());
        auto current_line = new char[cnt + 1];
        input_grades.read(current_line, (static_cast<streamsize>(cnt) + 1) * sizeof(char));
        CryptDecrypt(hSessionKey, 0, true, 0, (BYTE*)current_line, &cnt);
        string str(current_line);
        stringstream line(str);
        line >> term >> record_book;
        for (short j = 0; j < (*Grades[static_cast<size_t>(term) - 1]->subjects).size(); j++) {
            line >> mark;
            Grades[static_cast<size_t>(term) - 1]->marks[record_book][(*Grades[static_cast<size_t>(term) - 1]->subjects)[j]] = mark;
        }
        delete [] current_line;
    }
}

void consoleHorizontalLine(int w) {
    cout.width(static_cast<streamsize>(w) + 1);
    cout.fill('-');
    cout << "-" << endl;
    cout.fill(' ');
}

bool checkGrades(const string& student_marks, int sub_cnt) {
    int cnt = 0;
    string variants[3] = { "Отл", "Хор", "Уд" }, mark;
    stringstream line(student_marks);
    while (line >> mark) {
        if (mark != variants[0] && mark != variants[1] && mark != variants[2]) {
            return false;
        }
        cnt++;
    }
    if (cnt != sub_cnt) return false;
    else return true;
}

string createDate(unsigned short day, unsigned short month, unsigned short year) {
    string date_day = (day > 9 ? (to_string(day) + '.') : ('0' + to_string(day) + '.'));;
    string date_month = (month > 9 ? (to_string(month) + '.') : ('0' + to_string(month) + '.'));;
    string date = date_day + date_month + to_string(year);
    return date;
};

class Student {
public:
    string group;
    string faculty;
    string department;
    string firstName;
    string lastName;
    string dadName;
    string gender;
    string birthDate;
    string recordBook;
    int firstYear;

    Student(string s_firstname, string s_lastName, string s_dadName, string s_gender, string s_birthDate, string s_group, string s_faculty,
        string s_department, string s_recordBook, int s_firstYear) {
        firstName = s_firstname;
        lastName = s_lastName;
        dadName = s_dadName;
        gender = s_gender;
        group = s_group;
        faculty = s_faculty;
        recordBook = s_recordBook;
        firstYear = s_firstYear;
        department = s_department;
        birthDate = s_birthDate;
    }

    void createConsoleStudentLine() const{
        cout.width(20);
        cout << left << "|" + lastName;
        cout << "|";
        cout.width(15);
        cout << left << firstName;
        cout << "|";
        cout.width(20);
        cout << left << dadName;
        cout << "|";
        cout.width(5);
        cout << left << gender;
        cout << "|";
        cout.width(19);
        cout << left << birthDate;
        cout << "|";
        cout.width(10);
        cout << left << group;
        cout << "|";
        cout.width(11);
        cout << right << firstYear;
        cout << "|";
        cout.width(19);
        cout << right << recordBook;
        cout << "|";
        cout.width(10);
        cout << right << faculty;
        cout << "|";
        cout.width(9);
        cout << right << department;
        cout << "|" << endl;
        consoleHorizontalLine(147);
    }

    string createInputStudentLine() const{
        return lastName + ' ' + firstName + ' ' + dadName + ' ' + gender + ' ' + birthDate + ' ' + group + ' ' + recordBook + ' ' + faculty + ' ' + department;
    }
};

bool studentComp(Student& st1, Student& st2) {
    return st1.lastName < st2.lastName;
}

bool containsOnlyLetters(string const& word) {
    string all_letters = "ёйцукенгшщзхъфывапролджэячсмитьбюЁЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ";
    return word.find_first_not_of(all_letters) == string::npos;
}

bool checkGroup(string group) {
    int firstYear;
    string tmp = group;
    string letters = "ЁЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ";
    string nums = "0123456789";
    for (char& i : group) {
        if (letters.find(i) != string::npos) {
            i = '?';
        }
        else if (nums.find(i) != string::npos) {
            i = '*';
        }
        else if (i == '-') {
            continue;
        }
        else {
            return false;
        }
    }
    if (group == "????-**-**") {
        firstYear = stoi("20" + tmp.substr(8, 2));
        return ((firstYear <= 2023) && (firstYear >= 2016));
    }
    return false;
}

bool checkFaculty(const string& faculty) {
    return (find(all_faculties.begin(), all_faculties.end(), faculty) != all_faculties.end());
}

bool checkDepartment(string& department) {
    string all_symbols = "ёйцукенгшщзхъфывапролджэячсмитьбюЁЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ0123456789-";
    for (char& i : department) {
        if (all_symbols.find(i) == string::npos) {
            return false;
        }
    }
    return (department.size() < 9);
}

bool checkMark(const string& mark) {
    vector<string> marks = { "Отл", "Хор", "Уд" };
    return (find(marks.begin(), marks.end(), mark) != marks.end());
}

bool checkRecordBook(string record_book) {
    string letters = "ЁЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ";
    string nums = "0123456789";
    for (char& i : record_book) {
        if (letters.find(i) != string::npos) {
            i = '?';
        }
        else if (nums.find(i) != string::npos) {
            i = '*';
        }
        else {
            return false;
        }
    }
    return (record_book == "**?****");
}

void fileStudentsRefill(bool encryptionStatus) {
    ofstream students_list("input.txt", ios_base::trunc);
    ifstream head("studentsListHeader.txt");
    string student_line;

    if (encryptionStatus) {
        for (auto& k : v_students) {
            student_line = k.createInputStudentLine();
            DWORD count = static_cast<DWORD>(student_line.size());
            CryptEncrypt(hSessionKey, 0, true, 0, (BYTE*)student_line.c_str(), &count, static_cast<DWORD>(student_line.size()));
            students_list << student_line << endl;
        }
    }
    else {
        for (auto& k : v_students) {
            students_list << k.createInputStudentLine() << endl;
        }
    }

    students_list.close();
}

void fileGradesRefill(bool encryptionStatus) {
    ofstream file_grades("grades.txt", ios_base::trunc);
    string grades_line;
    if (encryptionStatus) {
        for (int i = 0; i < 9; i++) {
            for (auto iter1 = Grades[i]->marks.begin(); iter1 != Grades[i]->marks.end(); iter1++) {
                grades_line = to_string(i + 1) + (*iter1).first;
                for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); iter2++) { 
                    grades_line += ' ' + iter2->second;
                }
                DWORD count = static_cast<DWORD>(grades_line.size());
                CryptEncrypt(hSessionKey, 0, true, 0, (BYTE*)grades_line.c_str(), &count, static_cast<DWORD>(grades_line.size()));
                file_grades  << grades_line << endl;
            }
        }
    }
    else {
        for (int i = 0; i < 9; i++) {
            for (auto iter1 = Grades[i]->marks.begin(); iter1 != Grades[i]->marks.end(); iter1++) {
                file_grades << i + 1 << ' ' << (*iter1).first;
                for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); iter2++) {
                    file_grades << ' ' << iter2->second;
                }
                file_grades << endl;
            }
        }
    }
    file_grades.close();
}

void changeLastName(vector <Student>::iterator it) {
    string lastName;
    string record_book = (*it).recordBook;
    cout << "Введите фамилию студента." << endl;
    getline(cin, lastName);
    if (lastName.empty()) getline(cin, lastName);
    while (!containsOnlyLetters(lastName) || lastName.size() > 19) {
        cerr << "Некорректный ввод. Попробуйте ещё раз." << endl;
        getline(cin, lastName);
    }
    lastName[0] = toupper(lastName[0]);
    transform(lastName.begin() + 1, lastName.end(), lastName.begin() + 1, [](char c) {return tolower(c); });
    (*it).lastName = lastName;
    sort(v_students.begin(), v_students.end(), studentComp);
    fileStudentsRefill(true);
    cout << "Фамилия изменена успешно!" << endl;
    auto predicate = [record_book](const Student& item) {
        return item.recordBook == record_book;
    };
    it = find_if(begin(v_students), end(v_students), predicate);
    printExitMenu(it);
}

void changeFirstName(vector <Student>::iterator it) {
    string firstName;
    cout << "Введите имя студента." << endl;
    getline(cin, firstName);
    if (firstName.empty()) getline(cin, firstName);
    while (!containsOnlyLetters(firstName) || firstName.size() > 14) {
        cerr << "Некорректный ввод. Попробуйте ещё раз." << endl;
        getline(cin, firstName);
    }
    firstName[0] = toupper(firstName[0]);
    transform(firstName.begin() + 1, firstName.end(), firstName.begin() + 1, [](char c) {return tolower(c); });
    (*it).firstName = firstName;
    fileStudentsRefill(true);
    cout << "Имя изменено успешно!" << endl;
    printExitMenu(it);
}

void changeDadName(vector <Student>::iterator it) {
    string dadName;
    cout << "Введите отчество студента." << endl;
    getline(cin, dadName);
    if (dadName.empty()) getline(cin, dadName);
    while (!containsOnlyLetters(dadName) || dadName.size() > 19) {
        cerr << "Некорректный ввод. Попробуйте ещё раз." << endl;
        getline(cin, dadName);
    }
    dadName[0] = toupper(dadName[0]);
    transform(dadName.begin() + 1, dadName.end(), dadName.begin() + 1, [](char c) {return tolower(c); });
    (*it).dadName = dadName;
    fileStudentsRefill(true);
    cout << "Отчество изменено успешно!" << endl;
    printExitMenu(it);
}

void changeGender(vector <Student>::iterator it) {
    (*it).gender = ((*it).gender == "м") ? "ж" : "м";
    fileStudentsRefill(true);
    cout << "Пол изменён успешно!" << endl;
    printExitMenu(it);
}

bool checkDate(string input) {
    string variants = "0123456789 ";
    int day, month, year;
    if (count(input.begin(), input.end(), ' ') == 2 && input.find_first_not_of(variants) == string::npos) {
        stringstream line(input);
        line >> day >> month >> year;
        if ((day < 32) && (day > 0) && (month < 13) && (month > 0)) {
            return true;
        }
        else {
            return false;
        }
    }
    else return false;
}

void changeBirthDate(vector <Student>::iterator it) {
    string input;
    unsigned short day, month, year;
    string birthDate;
    cout << "Введите дату рождения в формате Ч М ГГГГ" << endl;
    getline(cin, input);
    if (input.empty()) getline(cin, input);
    while (!checkDate(input)) {
        cerr << "Некорректный ввод. Попробуйте ещё раз." << endl;
        getline(cin, input);
    }   
    stringstream date(input);
    date >> day >> month >> year;
    birthDate = createDate(day, month, year);
    (*it).birthDate = birthDate;
    fileStudentsRefill(true);
    cout << "Дата рождения изменёна успешно!" << endl;
    printExitMenu(it);       
}

void changeGroup(vector <Student>::iterator it) {
    string group;
    int firstYear;
    cout << "Введите номер группы в формате ББББ-ЦЦ-ЦЦ. Б-заглавная буква; Ц-цифра" << endl;
    getline(cin, group);
    if (group.empty()) getline(cin, group);
    while (!checkGroup(group)) {
        cerr << "Некорректный ввод. Попробуйте ещё раз." << endl;
        getline(cin, group);
    }
    firstYear = stoi("20" + group.substr(8, 2));
    (*it).group = group;
    (*it).firstYear = firstYear;
    fileStudentsRefill(true);
    cout << "Номер группы успешно изменён!" << endl;
    printExitMenu(it);
}

void changeFaculty(vector <Student>::iterator it) {
    string faculty;
    cout << "Введите аббревиатуру факультета (ИКБ, ИТУ, ИПТИП, ИИТ, ИИИ, ИТХТ, ИРИ)" << endl;
    getline(cin, faculty);
    if (faculty.empty()) getline(cin, faculty);
    while (!checkFaculty(faculty)) {
        cerr << "Такого факультета нет в списке. Попробуйте ещё раз." << endl;
        getline(cin, faculty);
    }
    (*it).faculty = faculty;
    fileStudentsRefill(true);
    cout << "Факультет успешно изменён!" << endl;
    printExitMenu(it);
}

void changeDepartment(vector <Student>::iterator it) {
    string department;
    cout << "Введите кафедру" << endl;
    getline(cin, department);
    if (department.empty()) getline(cin, department);
    while (!checkDepartment(department)) {
        cerr << "Некорректный ввод. Попробуйте заново" << endl;
        getline(cin, department);
    }
    (*it).department = department;
    fileStudentsRefill(true);
    cout << "Номер кафедры успешно изменён!" << endl;
    printExitMenu(it);
}

void changeRecordBook(vector <Student>::iterator it) {
    string record_book;
    cout << "Введите номер зачётной книжки (в формате ЦЦБЦЦЦЦ) Б-заглавная буква; Ц-цифра" << endl;
    getline(cin, record_book);
    if (record_book.empty()) getline(cin, record_book);
    while ((recordBooks.find(record_book) != recordBooks.end()) || (!checkRecordBook(record_book))) {
        if (recordBooks.find(record_book) != recordBooks.end()) {
            cerr << "Студент с таким номером зачётной книжки уже есть в списке. Введите повторно." << endl;
        }
        else {
            cerr << "Некорректный ввод. Введите повторно." << endl;
        }
        getline(cin, record_book);
    }
    for (int i = 0; i < 9; i++) {
        if (Grades[i]->marks.find((*it).recordBook) != Grades[i]->marks.end()) {
            map <string, string> tmp;
            for (auto j = Grades[i]->marks[(*it).recordBook].begin(); j != Grades[i]->marks[(*it).recordBook].end(); j++) {
                tmp[(*j).first] = (*j).second;
            }
            Grades[i]->marks.erase((*it).recordBook);
            for (auto iter = tmp.begin(); iter != tmp.end(); iter++) {
                Grades[i]->marks[record_book][(*iter).first] = (*iter).second;
            }
            tmp.clear();
        }
    }
    fileGradesRefill(true);
    recordBooks.erase((*it).recordBook);
    (*it).recordBook = record_book;
    recordBooks.insert((*it).recordBook);
    fileStudentsRefill(true);
    cout << "Номер зачётной книжки успешно изменён!" << endl;
    printExitMenu(it);
}

void changeMark(vector <Student>::iterator it) {
    string mark;
    string term_tag;
    int terms_count;
    terms_count = (((2023 - (*it).firstYear) * 2 + 1 > 9) ? 9 : (2023 - (*it).firstYear) * 2 + 1);
    vector <string> current_term_tags;
    vector <string> all_term_tags = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    cout << "За какой семестр вы хотите изменить оценку? Выберите:" << endl;
    for (int i = 1; i <= terms_count; i++) {
        cout << i << endl;
        current_term_tags.push_back(to_string(i));
    }
    getline(cin, term_tag);
    if (term_tag.empty()) getline(cin, term_tag);
    while (find(current_term_tags.begin(), current_term_tags.end(), term_tag) == current_term_tags.end()) {
        if (find(all_term_tags.begin(), all_term_tags.end(), term_tag) != all_term_tags.end()) {
            cerr << "За этот семестр у студента нет оценок. Введите повторно." << endl;
        }
        else {
            cerr << "Некорректный ввод. Попробуйте ещё раз" << endl;
        }
        getline(cin, term_tag);
    }
    int int_term_tag = stoi(term_tag);
    vector <string> current_subject_tags;
    cout << "По какому предмету вы хотите изменить оценку? Выберите:" << endl;
    for (int i = 0; i < (*Grades[static_cast<size_t>(int_term_tag) - 1]->subjects).size(); i++) {
        cout << i << ' ' << subjectsInterpretations[(*Grades[static_cast<size_t>(int_term_tag) - 1]->subjects)[i]] << endl;
        current_subject_tags.push_back(to_string(i));
    }
    string subject_tag;
    getline(cin, subject_tag);
    if (subject_tag.empty()) getline(cin, subject_tag);
    while (find(current_subject_tags.begin(), current_subject_tags.end(), subject_tag) == current_subject_tags.end()) {
        cerr << "Такого предмета нет в списке. Введите повторно." << endl;
        getline(cin, subject_tag);
    }
    int int_subject_tag = stoi(subject_tag);
    cout << "Введите оценку (Отл, Хор, Уд)." << endl;
    getline(cin, mark);
    if (mark.empty()) getline(cin, mark);
    while (!checkMark(mark)) {
        cerr << "Некорректно введена оценка. Введите повторно." << endl;
        getline(cin, mark);
    }
    Grades[static_cast<size_t>(int_term_tag) - 1]->marks[(*it).recordBook][(*Grades[static_cast<size_t>(int_term_tag) - 1]->subjects)[int_subject_tag]] = mark;
    fileGradesRefill(true);
    cout << "Оценка успешно изменена!" << endl;
    printExitMenu(it);
}

void printExitMenu(vector <Student>::iterator it) {
    string tag;
    vector <string> keys = { "0", "1" };
    cout << "0 - Выйти в главное меню" << endl;
    cout << "1 - Продолжить редактирование" << endl;
    getline(cin, tag);
    if (tag.empty()) getline(cin, tag);
    while (find(keys.begin(), keys.end(), tag) == keys.end()) {
        cerr << "Неверный параметр. Введите повторно." << endl;
        getline(cin, tag);
    }

    if (tag == "0") { printMainMenu(); }
    if (tag == "1") { printRefactorMenu(it); }
}

void printMainMenu() {
    string tag;
    vector <string> keys = { "0", "1", "2", "3", "4", "5", "6", "7", "q" };
    cout << "Чтобы вывести информацию о всех студентах введите (0)" << endl;
    cout << "Чтобы добавить студента введите (1)" << endl;
    cout << "Чтобы удалить студента введите (2)" << endl;
    cout << "Чтобы изменить данные студента введите (3)" << endl;
    cout << "Чтобы вывести отличников нажмите (4)" << endl;
    cout << "Чтобы вывести хорошистов нажмите (5)" << endl;
    cout << "Чтобы вывести троечников нажмите (6)" << endl;
    cout << "Чтобы показать успеваемость студента нажмите (7)" << endl;
    cout << "Чтобы завершить программу введите (q)" << endl;
    getline(cin, tag);
    if (tag.empty()) getline(cin, tag);
    while (find(keys.begin(), keys.end(), tag) == keys.end()) {
        cerr << "Неверный параметр. Введите заново." << endl;
        getline(cin, tag);
    }
    if (tag == "0") {
        ifstream students_header("studentsListHeader.txt");
        cout << students_header.rdbuf();
        for (auto it = v_students.begin(); it != v_students.end(); it++) {
            (*it).createConsoleStudentLine();
        }
        students_header.close();
        printMainMenu();
    }
    if (tag == "1") { addStudent(); printMainMenu(); }
    if (tag == "2") { deleteStudent(); printMainMenu(); }
    if (tag == "3") { refactorStudent(); }
    if (tag == "4") { findExcellent(); printMainMenu(); }
    if (tag == "5") { findGood(); printMainMenu(); }
    if (tag == "6") { findSatisfactory(); printMainMenu(); }
    if (tag == "7") { showStudentGrades(); printMainMenu(); }
    if (tag == "q") {
        fileStudentsRefill(false);
        fileGradesRefill(false);
        CryptDestroyKey(hSessionKey);
        CryptReleaseContext(hProv, 0); 
        exit(0);
    }
}

void printRefactorMenu(vector <Student>::iterator current_student_iterator) {
    string tag;
    vector <string> keys = { "0", "1", "2", "3", "4", "5", "6", "8", "9" };
    cout << "Выберите, какую информацию о студенте вы хотите изменить:" << endl;
    cout << "1 - Фамилию" << endl;
    cout << "2 - Имя" << endl;
    cout << "3 - Отчество" << endl;
    cout << "4 - Пол" << endl;
    cout << "5 - Дату рождения" << endl;
    cout << "6 - Группу" << endl;
    cout << "7 - Факультет" << endl;
    cout << "8 - Кафедру" << endl;
    cout << "9 - Номер зачётной книжки" << endl;
    cout << "0 - Успеваемость" << endl;
    getline(cin, tag);
    if (tag.empty()) getline(cin, tag);
    while (find(keys.begin(), keys.end(), tag) == keys.end()) {
        cerr << "Неверный параметр. Введите заново." << endl;
        getline(cin, tag);
    }
    if (tag == "0") changeMark(current_student_iterator);
    if (tag == "1") changeLastName(current_student_iterator);
    if (tag == "2") changeFirstName(current_student_iterator);
    if (tag == "3") changeDadName(current_student_iterator);
    if (tag == "4") changeGender(current_student_iterator);
    if (tag == "5") changeBirthDate(current_student_iterator);
    if (tag == "6") changeGroup(current_student_iterator);
    if (tag == "7") changeFaculty(current_student_iterator);
    if (tag == "8") changeDepartment(current_student_iterator);
    if (tag == "9") changeRecordBook(current_student_iterator);
}

void refactorStudent() {
    string record_book;
    cout << "Введите номер зачётной книжки студента, информацию о котором вы хотите изменить." << endl;
    getline(cin, record_book);
    if (record_book.empty()) getline(cin, record_book);
    while (recordBooks.find(record_book) == recordBooks.end()) {
        cerr << "Студента с таким номером зачётной книжки нет в списке. Введите повторно." << endl;
        getline(cin, record_book);
    }
    auto predicate = [record_book](const Student& item) {
        return item.recordBook == record_book;
        };
    auto current_student_iterator = find_if(begin(v_students), end(v_students), predicate);
    cout << "Информация о редактируемом студенте:" << endl;
    ifstream students_header("studentsListHeader.txt");
    cout << students_header.rdbuf();
    (*current_student_iterator).createConsoleStudentLine();
    students_header.close();
    printRefactorMenu(current_student_iterator);
}

void deleteStudent() {
    string record_book;
    int terms_count;

    cout << "Введите номер зачётной книжки студента, которого хотите удалить." << endl;
    getline(cin, record_book);
    if (record_book.empty()) getline(cin, record_book);
    while (recordBooks.find(record_book) == recordBooks.end()) {
        cerr << "Студента с таким номером зачётной книжки нет в списке. Введите повторно." << endl;
        getline(cin, record_book);
    }

    auto predicate = [record_book](const Student& item) {
        return item.recordBook == record_book;
        };
    auto current_student_iterator = find_if(begin(v_students), end(v_students), predicate);

    terms_count = (((2023 - (*current_student_iterator).firstYear) * 2 + 1 > 9) ? 9 : (2023 - (*current_student_iterator).firstYear) * 2 + 1);

    recordBooks.erase(record_book);
    for (int i = 0; i < terms_count; i++) {
        Grades[i]->marks.erase(record_book);
    }

    fileGradesRefill(true);
    v_students.erase(current_student_iterator);
    fileStudentsRefill(true);
    cout << "Вся информация о студенте удалена успешно!" << endl;
}

void addStudent() {
    string input_date;
    ofstream list("input.txt", ios_base::app),
        file_marks("grades.txt", ios_base::app);
    ifstream head("studentsListHeader.txt");
    string firstName, lastName, dadName, gender, recordBook, group, faculty, department, birthDate, student_marks, mark;
    unsigned short day, month, year, terms_count, s_cnt = 0;
    int firstYear;
    string student_line, grades_line;
    DWORD grades_count;
 
    cout << "Введите фамилию" << endl;
    getline(cin, lastName);
    if (lastName.empty()) getline(cin, lastName);
    while (!containsOnlyLetters(lastName) || lastName.size() > 19) {
        cerr << "Некорректный ввод. Попробуйте ещё раз." << endl;
        getline(cin, lastName);
    }
    lastName[0] = toupper(lastName[0]);
    transform(lastName.begin() + 1, lastName.end(), lastName.begin() + 1, [](char c) {return tolower(c); });
    
    cout << "Введите имя" << endl;
    getline(cin, firstName);
    if (firstName.empty()) getline(cin, firstName);
    while (!containsOnlyLetters(firstName) || firstName.size() > 14) {
        cerr << "Некорректный ввод. Попробуйте ещё раз." << endl;
        getline(cin, firstName);
    }
    firstName[0] = toupper(firstName[0]);
    transform(firstName.begin() + 1, firstName.end(), firstName.begin() + 1, [](char c) {return tolower(c); });
    
    cout << "Введите отчество" << endl;
    getline(cin, dadName);
    if (dadName.empty()) getline(cin, dadName);
    while (!containsOnlyLetters(dadName) || dadName.size() > 19) {
        cerr << "Некорректный ввод. Попробуйте ещё раз." << endl;
        getline(cin, dadName);
    }
    dadName[0] = toupper(dadName[0]);
    transform(dadName.begin() + 1, dadName.end(), dadName.begin() + 1, [](char c) {return tolower(c); });

    cout << "Введите пол (м или ж)" << endl;
    getline(cin, gender);
    if (gender.empty()) getline(cin, gender);
    while (gender != "м" && gender != "ж") {
        cerr << "Некорректный ввод. Введите повторно." << endl;
        getline(cin, gender);
    }
    cout << "Введите дату рождения в формате Ч М ГГГГ" << endl;
    getline(cin, input_date);
    if (input_date.empty()) getline(cin, input_date);
    while (!checkDate(input_date)) {
        cerr << "Некорректный ввод. Введите повторно." << endl;
        getline(cin, input_date);
    }
    stringstream s(input_date);
    s >> day >> month >> year;
    birthDate = createDate(day, month, year);
    cout << "Введите номер группы в формате ББББ-ЦЦ-ЦЦ. Б-заглавная буква; Ц-цифра" << endl;
    getline(cin, group);
    if (group.empty()) getline(cin, group);
    while (!checkGroup(group)) {
        cerr << "Некорректный ввод. Попробуйте ещё раз." << endl;
        getline(cin, group);
    }
    firstYear = stoi("20" + group.substr(8, 2));
    cout << "Введите номер зачётной книжки (в формате ЦЦБЦЦЦЦ) Б-заглавная буква; Ц-цифра" << endl;
    getline(cin, recordBook);
    if (recordBook.empty()) getline(cin, recordBook);
    while ((recordBooks.find(recordBook) != recordBooks.end()) || (!checkRecordBook(recordBook))) {
        if (recordBooks.find(recordBook) != recordBooks.end()) {
            cerr << "Студент с таким номером зачётной книжки уже есть в списке. Введите повторно." << endl;
        }
        else {
            cerr << "Некорректный ввод. Введите повторно." << endl;
        }
        getline(cin, recordBook);
    }
    cout << "Введите аббревиатуру факультета (ИКБ, ИТУ, ИПТИП, ИИТ, ИИИ, ИТХТ, ИРИ)" << endl;
    getline(cin, faculty);
    if (faculty.empty()) getline(cin, faculty);
    while (!checkFaculty(faculty)) {
        cerr << "Такого факультета нет в списке. Попробуйте ещё раз." << endl;
        getline(cin, faculty);
    }
    cout << "Введите название кафедры" << endl;
    getline(cin, department);
    if (department.empty()) getline(cin, department);
    while (!checkDepartment(department)) {
        cerr << "Некорректный ввод. Попробуйте заново" << endl;
        getline(cin, department);
    }

    terms_count = (((2023 - firstYear) * 2 + 1 > 9) ? 9 : (2023 - firstYear) * 2 + 1);
    for (int i = 0; i < terms_count; i++) {
        cout << "Введите оценки студента за " << i + 1 << "-й семестр (Отл, Хор, Уд) по " << (*Grades[i]->subjects).size() << "-и предметам из списка:" << endl;
        for (const auto& element : *Grades[i]->subjects) {
            cout << subjectsInterpretations[element] << endl;
        }
        getline(cin, student_marks);
        if (student_marks.empty()) {
            getline(cin, student_marks);
        }
        while (!checkGrades(student_marks, (int)(*Grades[i]->subjects).size())) {
            cerr << "Некорректно введены оценки! Попробуйте заново." << endl;
            getline(cin, student_marks);
        }
        stringstream line(student_marks);
        s_cnt = 0;
        grades_line = to_string(i + 1) + ' ' + recordBook;
        while (line >> mark) {
            grades_line += ' ' + mark;
            Grades[i]->marks[recordBook][(*Grades[i]->subjects)[s_cnt]] = mark;
            s_cnt++;
        }
        grades_count = static_cast<DWORD>(grades_line.size());
        CryptEncrypt(hSessionKey, 0, true, 0, (BYTE*)grades_line.c_str(), &grades_count, static_cast<DWORD>(grades_line.size()));
        file_marks << grades_line<< endl;
    }

    Student student(firstName, lastName, dadName, gender, birthDate, group, faculty, department, recordBook, firstYear);
    student_line = student.createInputStudentLine();
    DWORD student_count = static_cast<DWORD>(student_line.size());
    CryptEncrypt(hSessionKey, 0, true, 0, (BYTE*)student_line.c_str(), &student_count, static_cast<DWORD>(student_line.size()));
    list << student_line << endl;
    v_students.push_back(student);

    recordBooks.insert(student.recordBook);
    sort(v_students.begin(), v_students.end(), studentComp);
    cout << "Студент успешно добавлен!" << endl;
    file_marks.close();
    list.close();
    head.close();
}

void firstFill(vector<string> &student_lines) {
    ifstream input("input.txt", ios_base::binary);
    DWORD cnt;
    string firstName, lastName, dadName, gender, recordBook, group, faculty, department, birthDate;
    int firstYear;
    for (int i = 0; i < student_lines.size(); i++) {
        cnt = static_cast<DWORD>(student_lines[i].size());
        auto current_line = new char[cnt + 1];
        input.read(current_line, (static_cast<streamsize>(cnt) + 1) * sizeof(char));
        CryptDecrypt(hSessionKey, 0, true, 0, (BYTE*)current_line, &cnt);
        string str(current_line);
        stringstream line(str);
        line >> lastName >> firstName >> dadName >> gender >> birthDate >> group >> recordBook >> faculty
            >> department;
        firstYear = stoi("20" + group.substr(8, 2));
        Student st(firstName, lastName, dadName, gender, birthDate,
            group, faculty, department, recordBook,
            firstYear);
        v_students.push_back(st);
        recordBooks.insert(st.recordBook);
        delete[] current_line;
    }
    sort(v_students.begin(), v_students.end(), studentComp);
    input.close();
}
    
void findExcellent() {
    string term_input;
    vector <string> keys = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    cout << "За какой из 9-и семестров вы хотите посмотреть отличников?" << endl;
    getline(cin, term_input);
    while (find(keys.begin(), keys.end(), term_input) == keys.end()) {
        cerr << "Неверный параметр. Введите заново." << endl;
        getline(cin, term_input);
    }
    int term = stoi(term_input);
    string record_book;
    for (auto iter1 = Grades[static_cast<size_t>(term) - 1]->marks.begin(); iter1 != Grades[static_cast<size_t>(term) - 1]->marks.end(); iter1++) {
        bool flag = true;
        record_book = (*iter1).first;
        for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); iter2++) {
            if (iter2->second != "Отл") {
                flag = false;
                break;
            }
        }
        if (flag) {
            auto predicate = [record_book](const Student& item) {
                return item.recordBook == record_book;
                };
            auto current_student_iterator = find_if(begin(v_students), end(v_students), predicate);
            v_otl.push_back((*current_student_iterator));
        }
    }

    if (v_otl.empty()) { cout << "В " << term << "-м семестре отличников нет" << endl; }
    else {
        ifstream students_header("studentsListHeader.txt");
        cout << students_header.rdbuf();
        for (auto it = v_otl.begin(); it != v_otl.end(); it++) {
            (*it).createConsoleStudentLine();
        }
        students_header.close();
    }
    v_otl.clear();
}

void findGood() {
    string term_input;
    vector <string> keys = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    cout << "За какой из 9-и семестров вы хотите посмотреть хорошистов?" << endl;
    getline(cin, term_input);
    while (find(keys.begin(), keys.end(), term_input) == keys.end()) {
        cerr << "Неверный параметр. Введите заново." << endl;
        getline(cin, term_input);
    }
    int term = stoi(term_input);
    string record_book;
    for (auto iter1 = Grades[static_cast<size_t>(term) - 1]->marks.begin(); iter1 != Grades[static_cast<size_t>(term) - 1]->marks.end(); iter1++) {
        bool flag_hor = false;
        bool flag_ud = false;
        record_book = (*iter1).first;
        for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); iter2++) {
            if (iter2->second == "Уд") {
                flag_ud = true;
                break;
            }
            if (iter2->second == "Хор") {
                flag_hor = true;
            }
        }
        if (flag_hor && !flag_ud) {
            auto predicate = [record_book](const Student& item) {
                return item.recordBook == record_book;
                };
            auto current_student_iterator = find_if(begin(v_students), end(v_students), predicate);
            v_hor.push_back((*current_student_iterator));
        }
    }
    if (v_hor.empty()) { cout << "В " << term << "-м семестре хорошистов нет" << endl; }
    else {
        ifstream students_header("studentsListHeader.txt");
        cout << students_header.rdbuf();
        for (auto it = v_hor.begin(); it != v_hor.end(); it++) {
            (*it).createConsoleStudentLine();
        }
        students_header.close();
    }
    v_hor.clear();
}

void findSatisfactory() {
    string term_input;
    vector <string> keys = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
    cout << "За какой из 9-и семестров вы хотите посмотреть троечников?" << endl;
    getline(cin, term_input);
    while (find(keys.begin(), keys.end(), term_input) == keys.end()) {
        cerr << "Неверный параметр. Введите заново." << endl;
        getline(cin, term_input);
    }
    int term = stoi(term_input);
    string record_book;
    for (auto iter1 = Grades[static_cast<size_t>(term) - 1]->marks.begin(); iter1 != Grades[static_cast<size_t>(term) - 1]->marks.end(); iter1++) {
        bool flag = false;
        record_book = (*iter1).first;
        for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); iter2++) {
            if (iter2->second == "Уд") {
                flag = true;
                break;
            }
        }
        if (flag) {
            auto predicate = [record_book](const Student& item) {
                return item.recordBook == record_book;
                };
            auto current_student_iterator = find_if(begin(v_students), end(v_students), predicate);
            v_ud.push_back((*current_student_iterator));
        }
    }
    if (v_ud.empty()) { cout << "В " << term << "-м семестре троечников нет" << endl; }
    else {
        ifstream students_header("studentsListHeader.txt");
        cout << students_header.rdbuf();
        for (auto it = v_ud.begin(); it != v_ud.end(); it++) {
            (*it).createConsoleStudentLine();
        }
        students_header.close();
    }
    v_ud.clear();
}

void showStudentGrades() {
    size_t max_subject_width = 0;
    string record_book;
    string term_tag;
    vector <string> current_term_tags;
    vector <string> all_term_tags = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    cout << "Введите номер зачётной книжки студента." << endl;
    getline(cin, record_book);
    if (record_book.empty()) getline(cin, record_book);
    while (find(recordBooks.begin(), recordBooks.end(), record_book) == recordBooks.end()) {
        cerr << "Студента с таким номером зачётной книжки нет в списке. Введите заново" << endl;
        getline(cin, record_book);
    }
    auto predicate = [record_book](const Student& item) {
        return item.recordBook == record_book;
        };
    auto current_student_iterator = find_if(begin(v_students), end(v_students), predicate);
    int terms_count = (((2023 - (*current_student_iterator).firstYear) * 2 + 1 > 9) ? 9 : (2023 - (*current_student_iterator).firstYear) * 2 + 1);
    cout << "Выберите семестр, за который хотите просмотреть успеваемость:" << endl;
    for (int i = 1; i <= terms_count; i++) {
        cout << i << endl;
        current_term_tags.push_back(to_string(i));
    }
    getline(cin, term_tag);
    if (term_tag.empty()) getline(cin, term_tag);
    while (find(current_term_tags.begin(), current_term_tags.end(), term_tag) == current_term_tags.end()) {
        if (find(all_term_tags.begin(), all_term_tags.end(), term_tag) != all_term_tags.end()) {
            cerr << "За этот семестр у студента нет оценок. Введите повторно." << endl;
        }
        else {
            cerr << "Некорректный ввод. Попробуйте ещё раз" << endl;
        }
        getline(cin, term_tag);
    }
    int int_term_tag = stoi(term_tag);
    for (auto it = (*Grades[static_cast<size_t>(int_term_tag) - 1]->subjects).begin(); it != (*Grades[static_cast<size_t>(int_term_tag) - 1]->subjects).end(); it++) {
        if (subjectsInterpretations[(*it)].size() > max_subject_width) max_subject_width = subjectsInterpretations[(*it)].size();
    }
    consoleHorizontalLine(static_cast<int>(max_subject_width + 6 + 7));
    for (auto it = Grades[static_cast<size_t>(int_term_tag) - 1]->marks[record_book].begin(); it != Grades[static_cast<size_t>(int_term_tag) - 1]->marks[record_book].end(); it++) {
        cout.width(max_subject_width + 4);
        cout << left << "|" + subjectsInterpretations[(*it).first];
        cout << "|";
        cout.width(8);
        cout << right << (*it).second;
        cout << "|" << endl;
        consoleHorizontalLine(static_cast<int>(max_subject_width + 6 + 7));
    }
}

static void firstFilesEncrypt() {
    ifstream input_students("input.txt");
    ifstream input_grades("grades.txt");
    long long grades_count_line = count(istreambuf_iterator<char>(input_grades), istreambuf_iterator<char>(), '\n');
    long long students_count_line = count(istreambuf_iterator<char>(input_students), istreambuf_iterator<char>(), '\n');
    input_grades.seekg(0);
    input_students.seekg(0);
    vector <string> student_lines, grade_lines;
    string x;
    DWORD count;
    for (long long i = 0; i < grades_count_line; i++) {
        getline(input_grades, x);
        grade_lines.push_back(x);
    }
    for (long long i = 0; i < students_count_line; i++) {
        getline(input_students, x);
        student_lines.push_back(x);
    }
    input_students.close();
    input_grades.close();

    ofstream output_students("input.txt", ios_base::binary);
    ofstream output_grades("grades.txt", ios_base::binary);
    
    for (auto& s : student_lines) {
        count = static_cast<DWORD>(s.size());
        auto current = new char[count + 1];
        strcpy_s(current, (count) * sizeof(char) + 1, s.c_str());
        CryptEncrypt(hSessionKey, 0, true, 0, (BYTE*)current, &count, static_cast<DWORD>(s.size()));
        output_students.write(current, (static_cast<streamsize>(count) + 1) * sizeof(char));
        delete[] current;
    }

    for (auto& g : grade_lines) {
        count = static_cast<DWORD>(g.size());
        auto current = new char[count + 1];
        strcpy_s(current, (count) * sizeof(char) + 1, g.c_str());
        CryptEncrypt(hSessionKey, 0, true, 0, (BYTE*)current, &count, static_cast<DWORD>(g.size()));
        output_grades.write(current, (static_cast<streamsize>(count) + 1) * sizeof(char));
        delete[] current;
    }

    output_students.close();
    output_grades.close();

    firstFill(student_lines);
    gradesFill(grade_lines);
}

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        cout << "Encryption context has initialised successfully!" << endl;
    }
    if (CryptGenKey(hProv, CALG_RC4, CRYPT_EXPORTABLE, &hSessionKey)) {
        cout << "Session key has generated successfully!" << endl;
    }
    createGrades();
    firstFilesEncrypt();
    printMainMenu();
}