
#if !defined(AFX_INI_H__EE5B3831_1B1A_4E69_8478_6BA2E597A100__INCLUDED_)
#define AFX_INI_H__EE5B3831_1B1A_4E69_8478_6BA2E597A100__INCLUDED_

#pragma once

#include <stdio.h>
#include <vector>
#include <string>


#ifdef WIN32
#define UTILITY_EXPORT _declspec(dllexport)
#else
#define UTILITY_EXPORT
#endif

namespace Utility
{

	class  UTILITY_EXPORT CIniMgr
	{
	public:

		CIniMgr();

		bool InitFile(const char *pFilePath);
		
		void CloseFile();

		virtual ~CIniMgr();

		/*************************************************
	    Function:       GetPrivateProfileString
	    Description:    ������ô��������ú��������������windows api�ĺ���һ�£�
	    Calls:
	    Called By:
	    Input:          pSection������
						pKey���ؼ���
						pDefault��Ĭ��ֵ
						uiSize��pDestBuf�Ĵ�С
						pFileName�������ã���Ϊƥ��windows api��

	    Output:			pDestBuf������ȡ����ֵ
	    Return:			���������ֽ�����0��ʾû��Ӧ������
		*************************************************/
		unsigned int GetProfileStr(const char *pSection,const char *pKey,const char *pDefault,
			char *pDestBuf,unsigned int uiSize,const char *pFileName = NULL);

	protected:
		FILE *m_pFile;

		//m_SectionMap��������Ϊkey�����µ�����������ɵ�map����������Ϊ��map��key,����ֵ��Ϊ��map��ֵ����Ϊֵ
		//map<string,map<string,char *>*>	m_SectionMap;
		
		//ini���������Ϣ
		typedef struct tagCfgInf
		{
			char	*key;
			char	*value;
			struct tagCfgInf *next;
		}CFGINF,*PCFGINF;

		//ini���öε���Ϣ
		typedef struct tagSectionInf
		{
			char	*section;
			PCFGINF	cfgHead;
			PCFGINF	cfgTail;
			struct tagSectionInf *next;
		}SECTIONINF,*PSECTIONINF;

		PSECTIONINF	m_pAllCfgInf;//����ȫ����������Ϣ
		PSECTIONINF	m_pAllCfgInfTail;//����ȫ����������Ϣ�����β��

		/*************************************************
	    Function:       TrimLeft
	    Description:    ȥ���ַ�����ߵĿո�
	    Calls:
	    Called By:
	    Input:          pSrc��ԭ�ַ���

	    Output:
	    Return:			ȥ���ո���ƫ��ָ�루ֻ��ָ�����㣬���ݲ��ƶ���
		*************************************************/
		const char *TrimLeft(const char *pSrc);

		/*************************************************
	    Function:       Trim
	    Description:    ȥ���ַ������ҿո񼰽����Ļ��з�
	    Calls:
	    Called By:
	    Input:          pSrc��ԭ�ַ���

	    Output:
	    Return:			ȥ���ո���ƫ��ָ�루ֻ��ָ�����㣬���ݲ��ƶ���
		*************************************************/
		void Trim(char *pSrc);

		/*************************************************
	    Function:       GetSection
	    Description:    ��һ���ַ���(�ַ���ǰ�����пո�)�з��������öεĶ���
	    Calls:
	    Called By:
	    Input:          pLine���ַ���

	    Output:
	    Return:			���ض����ַ�ָ�룬NULL���ʾ�Ҳ�����
		*************************************************/
		char *GetSection(const char *pLine);

		/*************************************************
	    Function:       AnalyseCFG
	    Description:    ��һ���ַ����з�����������
	    Calls:
	    Called By:
	    Input:          pLine���ַ���

	    Output:			pSectionData�����������ֵ��ɵ�map
	    Return:			true�������ɹ�,false������ʧ��
		*************************************************/
		bool AnalyseCFG(const char *pLine,struct tagSectionInf **pSectionData);

	};

	std::vector<std::string> split(const std::string &str, const std::string &pattern);

};

#endif // !defined(AFX_INI_H__EE5B3831_1B1A_4E69_8478_6BA2E597A100__INCLUDED_)
