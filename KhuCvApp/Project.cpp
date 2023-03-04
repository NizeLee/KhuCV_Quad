//  Project.cpp: implementation of CProject (main project class you will write)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "KhuCvApp.h"
#include "Project.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif

int g_classNum;
wxColour g_classColor[2000];
wxString g_className[2000];

CProject::CProject() {
	GetExecutionPath();

	char InfoPath[256];
	strcpy(InfoPath, m_ExePath);
	strcat(InfoPath, "\\LabelInfo.txt");

	std::ifstream ifs(InfoPath);
	if (ifs) {
		ifs >> g_classNum;

		if (g_classNum > 2000) {
			g_classNum = 0;
		}

		for (int i = 0; i < g_classNum; ++i) {
			std::string name;
			int R, G, B;
			ifs >> name >> R >> G >> B;

			g_classColor[i] = wxColor(R, G, B);
			g_className[i] = name;
		}

		ifs.close();
	}
}

CProject::~CProject() {
}

void CProject::GetExecutionPath() {
	wxFileName f(wxStandardPaths::Get().GetExecutablePath());
	wxString appPath(f.GetPath());

	wcscpy(m_ExePathUnicode, appPath);
	strcpy(m_ExePath, appPath.c_str());
}

void CProject::Run(cv::Mat Input, cv::Mat& Output, bool bFirstRun, bool bVerbose) {
	///// Example code - Begin

	///// Example code - End
}

