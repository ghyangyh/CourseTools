#include <iostream>
#include <locale>
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;
namespace fs = std::filesystem;

// The locale to print Chinese
locale loc("chs");

/* This function will load a file containing Chinese.
*  A Chinese character is a wide char.
*/ 
bool load_ansi_chinese_file(const wstring& aFileName, vector<wstring>& aNameList) {
	wifstream inputs(aFileName);
	inputs.imbue(loc);
	if (inputs) {
		wcout << "The file " << aFileName << " opend! \n";
		wstring a_line;
		while (!inputs.eof()) {
			getline(inputs, a_line);
			if (!a_line.empty()) {
				aNameList.emplace_back(a_line);
			}
				
		}
		return true;
	}
	return false;
}

/*This function will extract a list of Chinese name strings from the directory path.
*/
void extract_name_wstr_from_files(const fs::path& aPath,
	const wstring& aOutputFileName,
	vector<wstring>& aNameList) {

	wofstream wfout(aOutputFileName);
	wfout.imbue(loc);
	// Iterate through the directoy specified by the path
	for (auto& e : fs::directory_iterator(aPath)) { // e is a directory entry object
		if (e.is_regular_file()) { // only handle a regular file
			fs::path a_file_name = e.path().filename(); // get the file full path
			wstring a_file_name_str = a_file_name.wstring(); // convert to a string

			/* Split the names and store them to the output file
			*/
			int start_pos = a_file_name_str.find_first_of(L"_") + 1;
			int stop_pos = a_file_name_str.find_last_of(L"_");
			wstring student_name = a_file_name_str.substr(start_pos, stop_pos - start_pos);
			wfout << student_name << endl;
			aNameList.emplace_back(student_name);
		}
	}
}

/* This function will check the unsubmitted students name list.
*/
void check_submit(const vector<wstring>& aSubmitStudentList,
	const vector<wstring>& aFullStudentList, 
	const wstring& aOutputFileName) {
	wofstream woutputs(aOutputFileName);
	woutputs.imbue(loc);
	wcout << L"未提交的同学名单：" << endl;
	for (const auto& s : aFullStudentList) {
		if (find(aSubmitStudentList.begin(),
			aSubmitStudentList.end(), s) ==
			aSubmitStudentList.end()) {
			woutputs << s << endl;
			wcout << s << endl;
		}
	}
}

/*Load the configure file to start checking procedure.
*/
bool load_configure_file(const string& aFileName, 
						vector<wstring>& aReportPaths, 
						vector<wstring>& aStudentListPaths,
						vector<wstring>& aOutpuPaths) {
	wifstream wfin(aFileName);
	wfin.imbue(loc);
	if (wfin) {
		wstring a_line;
		int line_count(0);
		while (!wfin.eof()) {
			getline(wfin, a_line);
			if (!a_line.empty()) {
				++line_count;
				if (line_count == 1) // the first line is a comment line
					continue;
				else { // store all the tasks' paths
					wstringstream wss(a_line);
					wstring report_path, student_list_path, output_path;
					wss >> report_path >> student_list_path >> output_path;
					aReportPaths.emplace_back(report_path);
					aStudentListPaths.emplace_back(student_list_path);
					aOutpuPaths.emplace_back(output_path);
				}
			}
		}
		return true;
	}
	return false;
}

int main(int argc, char** argv) {
	wcout.imbue(loc);
	if (argc < 2) {
		cout << "Usage: \n";
		cout << argv[0] << " input_file_name\n";
		return 1;
	}
	string configure_file_name(argv[1]);
	vector<wstring> a_report_path_list, a_full_student_path_list, a_output_path_list;
	
	if (!load_configure_file(configure_file_name,
		a_report_path_list,
		a_full_student_path_list,
		a_output_path_list)) {
		wcout << L"配置文件读取失败！" << endl;
		return 1;
	}
	for (size_t i = 0; i < a_report_path_list.size(); ++i) {
		wstring report_path = a_report_path_list[i];
		wstring full_student_list_file_path = a_full_student_path_list[i];
		wstring output_file_path = a_output_path_list[i];

		//wcout << L"报告文件夹路径：" << report_path << endl;
		//wcout << L"学生名单路径：" << full_student_list_file_path << endl;
		
		// The target path
		fs::path a_path(report_path);
		cout << "Path: " << a_path << endl;

		// Extract the output filename from the path
		//wstring a_path_string = a_path.wstring();
		// int start_pos = report_path.find_last_of(L"\\");
		// wstring a_file_name = report_path.substr(start_pos + 1);
		// wstring a_output_file_name = a_path.parent_path().wstring() + L"\\" + a_file_name + L".txt";
		
		// Extract the submitted students' name string list
		vector<wstring> submitted_student_list;
		extract_name_wstr_from_files(a_path, output_file_path, submitted_student_list);
		
		// Load the full students' name list
		vector<wstring> full_student_list;
		load_ansi_chinese_file(full_student_list_file_path, full_student_list);
		
		// Check submission
		check_submit(submitted_student_list, 
					full_student_list, 
					output_file_path);
	}

	/*int exit_code;
	cin >> exit_code;*/
	return 0;
}