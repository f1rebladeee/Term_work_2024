#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <windows.h>
#include <map>
#include <sstream>
#include <set>
#include <wincrypt.h>
#include <cstdio>

using namespace std;

///Структуры данных--------------------------------------------------------------
class Student; /* Класс, содержащий всю информацию о студенте */
struct Term; /* Структура, содержащая успеваемость всех студентов за 1 семестр */
class LinkedList; /* Класс связный список */
///------------------------------------------------------------------------------

///Глобальные переменные---------------------------------------------------------------------------------------
extern set <string> recordBooks; /* Множество всех зачётных книжек студентов */
extern LinkedList Grades; /* Связный список, содержащий всю успеваемость */
extern vector<Student> v_students, v_otl, v_hor, v_ud; /* Вектор всех студентов */
//extern DWORD dwIndex = 0;
//extern DWORD dwType = 0;
//extern DWORD caName = 0;
extern LPTSTR pszName, x;
extern HCRYPTPROV hProv;
extern HCRYPTKEY hSessionKey;
///------------------------------------------------------------------------------------------------------------

///Функции проверки корректности введённых с клавиатуры данных-------------------------------------------------
bool checkDate(string input); /* Проверка даты */
bool checkGrades(const string& student_marks, int sub_cnt); /* Проверка последовательности оценок за семестр */
bool containsOnlyLetters(string const& word); /* Проверка на присутствие в строке цифр или иных символов */
bool checkGroup(string group); /* Проверка группы */
bool checkFaculty(const string& faculty); /* Проверка факультета */
bool checkDepartment(string& department); /* Проверка кафедры */
bool checkMark(const string& mark); /* Проверка оценки */
bool checkRecordBook(string record_book); /* Проверка зачётной книжки */
///------------------------------------------------------------------------------------------------------------

///Функции для различных действий со студентами---------------------------------------------------------------
void addStudent(); /* Добавление студента */
void deleteStudent(); /* Удаление студента */
void refactorStudent(); /* Изменение информации о студенте */
void changeLastName(vector <Student>::iterator it); /* Изменение фамилии */
void changeFirstName(vector <Student>::iterator it); /* Изменение имени */
void changeDadName(vector <Student>::iterator it); /* Изменение отчества */
void changeGender(vector <Student>::iterator it); /* Изменение пола */
void changeBirthDate(vector <Student>::iterator it); /* Изменение даты рождения */
void changeGroup(vector <Student>::iterator it); /* Изменение группы */
void changeFaculty(vector <Student>::iterator it); /* Изменение факультета */
void changeDepartment(vector <Student>::iterator it); /* Изменение кафедры */
void changeRecordBook(vector <Student>::iterator it); /* Изменение номера зачётной книжки */
void changeMark(vector <Student>::iterator it); /* Изменение оценки в конкретном семестре по 1 предмету */
void findSatisfactory(); /* Вывод троечников */
void findGood(); /* Вывод хорошистов */
void findExcellent(); /* Вывод отличников */
void showStudentGrades(); /* Вывод в консоль успеваемости студента за 1 семестр */
///-----------------------------------------------------------------------------------------------------------

/// Функции первичного заполнения данных из файлов-----------------------
void firstFill(vector <string> &student_lines); /* Заполнение данных о студентах */
void gradesFill(vector <string> &grade_lines); /* Заполнение успеваемости студентов за 9 семестров */
///----------------------------------------------------------------------

///Функции отображения различных меню---------------------------------------
void printMainMenu(); /* Функция отображения главного меню */
void printRefactorMenu(vector <Student>::iterator current_student_iterator);
void printExitMenu(vector <Student>::iterator it);
///-------------------------------------------------------------------------

///Прочее----------------------------------------------------------------------------------------------------------
void fileGradesRefill(bool encryptionStatus); /* Функция перезаписи вводного файла с успеваемостью */
void fileStudentsRefill(bool encryptionStatus); /* Функция перезаписи вводного файла с информацией о студентах */
bool studentComp(Student& st1, Student& st2); /* Компаратор для сортировки студентов по алфавиту */
string createDate(unsigned short day, unsigned short month, unsigned short year); /* Функция форматирования даты */
void consoleHorizontalLine(int w); /* Функция отрисовки горизонтальной линии в консоль */
void createGrades(); /* Заполнение связного списка 9-ю структурами Term */
///----------------------------------------------------------------------------------------------------------------

///Шифрование файлов----------------------------------------------------------------------------
static void firstFilesEncrypt(); /* Первичное шифрование файлов со студентами и успеваемостью */
///---------------------------------------------------------------------------------------------