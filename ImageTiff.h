#pragma once
#include <iostream>
class AFX_EXT_CLASS CImageTiff
{
public:
	CImageTiff();
	~CImageTiff();

public:
	BOOL LoadTiffFile(const CString& strPath);
	BOOL LoadTiffFile(const CString& strPath, int* pImage, int nImageSize);
	BOOL LoadTiffFile(const CString& strPath, int*& pImage, int& nWidth, int& nHeight);

	BOOL LoadTiffFile(const CString& strPath, UINT16* pImage, int nImageSize);
	BOOL LoadTiffFile(const CString& strPath, UINT16*& pImage);
	BOOL LoadTiffFile(const CString& strPath, UINT16*& pImage, int& nWidth, int& nHeight);
	BOOL LoadTiffFile(const CString& strPath, UINT16*& pImage, int& nWidth, int& nHeight, int nRGBBit);
	BOOL LoadTiffFile(const CString& strPath, BYTE*& pImage, int& nWidth, int& nHeight);
	//BOOL LoadTiffFile(const CString& strPath, UINT16*& pImage, int& nWidth, int& nHeight, int& nImageSize);
	
	BOOL SaveTiffFile(CString strPath, int* pImage, int nWidth, int nHeight);
	BOOL SaveTiffFile(CString strPath, UINT16* pImage, int nWidth, int nHeight);
	BOOL SaveTiffFile(CString strPath, float* pImage, int nWidth, int nHeight);
	BOOL SaveTiffFile(CString strPath, unsigned char* pImage, int nWidth, int nHeight);

	inline UINT16*  GetImage(void) { return m_pImage; }
	inline int		GetWidth(void) { return m_nWidth; }
	inline int		GetHeight(void) { return m_nHeight; }
	//inline int		GetImageSize(void) { return m_nImageSize; }
private:
	UINT16*	m_pImage;
	int		m_nWidth;
	int		m_nHeight;
	//int     m_nImageSize;
};


