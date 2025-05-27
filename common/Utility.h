
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
	    Description:    获得配置串（其作用和其各个参数，和windows api的函数一致）
	    Calls:
	    Called By:
	    Input:          pSection：段名
						pKey：关键字
						pDefault：默认值
						uiSize：pDestBuf的大小
						pFileName：无作用（仅为匹配windows api）

	    Output:			pDestBuf：保存取到的值
	    Return:			被拷贝的字节数，0表示没对应配置项
		*************************************************/
		unsigned int GetProfileStr(const char *pSection,const char *pKey,const char *pDefault,
			char *pDestBuf,unsigned int uiSize,const char *pFileName = NULL);

	protected:
		FILE *m_pFile;

		//m_SectionMap：段名作为key，段下的所有配置组成的map（配置名作为此map的key,配置值作为此map的值）作为值
		//map<string,map<string,char *>*>	m_SectionMap;
		
		//ini配置项的信息
		typedef struct tagCfgInf
		{
			char	*key;
			char	*value;
			struct tagCfgInf *next;
		}CFGINF,*PCFGINF;

		//ini配置段的信息
		typedef struct tagSectionInf
		{
			char	*section;
			PCFGINF	cfgHead;
			PCFGINF	cfgTail;
			struct tagSectionInf *next;
		}SECTIONINF,*PSECTIONINF;

		PSECTIONINF	m_pAllCfgInf;//保存全部的配置信息
		PSECTIONINF	m_pAllCfgInfTail;//保存全部的配置信息链表的尾部

		/*************************************************
	    Function:       TrimLeft
	    Description:    去除字符串左边的空格
	    Calls:
	    Called By:
	    Input:          pSrc：原字符串

	    Output:
	    Return:			去除空格后的偏移指针（只做指针运算，数据不移动）
		*************************************************/
		const char *TrimLeft(const char *pSrc);

		/*************************************************
	    Function:       Trim
	    Description:    去除字符串左右空格及结束的换行符
	    Calls:
	    Called By:
	    Input:          pSrc：原字符串

	    Output:
	    Return:			去除空格后的偏移指针（只做指针运算，数据不移动）
		*************************************************/
		void Trim(char *pSrc);

		/*************************************************
	    Function:       GetSection
	    Description:    从一行字符串(字符串前不能有空格)中分析出配置段的段名
	    Calls:
	    Called By:
	    Input:          pLine：字符串

	    Output:
	    Return:			返回段名字符指针，NULL则表示找不到段
		*************************************************/
		char *GetSection(const char *pLine);

		/*************************************************
	    Function:       AnalyseCFG
	    Description:    从一行字符串中分析出配置项
	    Calls:
	    Called By:
	    Input:          pLine：字符串

	    Output:			pSectionData：由配置项及其值组成的map
	    Return:			true：分析成功,false：分析失败
		*************************************************/
		bool AnalyseCFG(const char *pLine,struct tagSectionInf **pSectionData);

	};

	std::vector<std::string> split(const std::string &str, const std::string &pattern);

};

#endif // !defined(AFX_INI_H__EE5B3831_1B1A_4E69_8478_6BA2E597A100__INCLUDED_)
