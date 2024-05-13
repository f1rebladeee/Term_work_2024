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

///��������� ������--------------------------------------------------------------
class Student; /* �����, ���������� ��� ���������� � �������� */
struct Term; /* ���������, ���������� ������������ ���� ��������� �� 1 ������� */
class LinkedList; /* ����� ������� ������ */
///------------------------------------------------------------------------------

///���������� ����������---------------------------------------------------------------------------------------
extern set <string> recordBooks; /* ��������� ���� �������� ������ ��������� */
extern LinkedList Grades; /* ������� ������, ���������� ��� ������������ */
extern vector<Student> v_students, v_otl, v_hor, v_ud; /* ������ ���� ��������� */
//extern DWORD dwIndex = 0;
//extern DWORD dwType = 0;
//extern DWORD caName = 0;
extern LPTSTR pszName, x;
extern HCRYPTPROV hProv;
extern HCRYPTKEY hSessionKey;
///------------------------------------------------------------------------------------------------------------

///������� �������� ������������ �������� � ���������� ������-------------------------------------------------
bool checkDate(string input); /* �������� ���� */
bool checkGrades(const string& student_marks, int sub_cnt); /* �������� ������������������ ������ �� ������� */
bool containsOnlyLetters(string const& word); /* �������� �� ����������� � ������ ���� ��� ���� �������� */
bool checkGroup(string group); /* �������� ������ */
bool checkFaculty(const string& faculty); /* �������� ���������� */
bool checkDepartment(string& department); /* �������� ������� */
bool checkMark(const string& mark); /* �������� ������ */
bool checkRecordBook(string record_book); /* �������� �������� ������ */
///------------------------------------------------------------------------------------------------------------

///������� ��� ��������� �������� �� ����������---------------------------------------------------------------
void addStudent(); /* ���������� �������� */
void deleteStudent(); /* �������� �������� */
void refactorStudent(); /* ��������� ���������� � �������� */
void changeLastName(vector <Student>::iterator it); /* ��������� ������� */
void changeFirstName(vector <Student>::iterator it); /* ��������� ����� */
void changeDadName(vector <Student>::iterator it); /* ��������� �������� */
void changeGender(vector <Student>::iterator it); /* ��������� ���� */
void changeBirthDate(vector <Student>::iterator it); /* ��������� ���� �������� */
void changeGroup(vector <Student>::iterator it); /* ��������� ������ */
void changeFaculty(vector <Student>::iterator it); /* ��������� ���������� */
void changeDepartment(vector <Student>::iterator it); /* ��������� ������� */
void changeRecordBook(vector <Student>::iterator it); /* ��������� ������ �������� ������ */
void changeMark(vector <Student>::iterator it); /* ��������� ������ � ���������� �������� �� 1 �������� */
void findSatisfactory(); /* ����� ���������� */
void findGood(); /* ����� ���������� */
void findExcellent(); /* ����� ���������� */
void showStudentGrades(); /* ����� � ������� ������������ �������� �� 1 ������� */
///-----------------------------------------------------------------------------------------------------------

/// ������� ���������� ���������� ������ �� ������-----------------------
void firstFill(vector <string> &student_lines); /* ���������� ������ � ��������� */
void gradesFill(vector <string> &grade_lines); /* ���������� ������������ ��������� �� 9 ��������� */
///----------------------------------------------------------------------

///������� ����������� ��������� ����---------------------------------------
void printMainMenu(); /* ������� ����������� �������� ���� */
void printRefactorMenu(vector <Student>::iterator current_student_iterator);
void printExitMenu(vector <Student>::iterator it);
///-------------------------------------------------------------------------

///������----------------------------------------------------------------------------------------------------------
void fileGradesRefill(bool encryptionStatus); /* ������� ���������� �������� ����� � ������������� */
void fileStudentsRefill(bool encryptionStatus); /* ������� ���������� �������� ����� � ����������� � ��������� */
bool studentComp(Student& st1, Student& st2); /* ���������� ��� ���������� ��������� �� �������� */
string createDate(unsigned short day, unsigned short month, unsigned short year); /* ������� �������������� ���� */
void consoleHorizontalLine(int w); /* ������� ��������� �������������� ����� � ������� */
void createGrades(); /* ���������� �������� ������ 9-� ����������� Term */
///----------------------------------------------------------------------------------------------------------------

///���������� ������----------------------------------------------------------------------------
static void firstFilesEncrypt(); /* ��������� ���������� ������ �� ���������� � ������������� */
///---------------------------------------------------------------------------------------------