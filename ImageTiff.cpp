// #include "pch.h"
#include "ImageTiff.h"
#include "tiffio.h"
#include <iostream>
#pragma comment(lib, "libtiff")


CImageTiff::CImageTiff()
{
	m_pImage = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	//m_nImageSize = 0;
}


CImageTiff::~CImageTiff()
{
	if (m_pImage)
	{
		delete[] m_pImage;
		m_pImage = NULL;
	}
}

BOOL CImageTiff::LoadTiffFile(const CString& strPath)
{
	return LoadTiffFile(strPath, m_pImage, m_nWidth, m_nHeight);
}
BOOL CImageTiff::LoadTiffFile(const CString& strPath, int* pImage, int nImageSize)
{
	BOOL bRet = FALSE;
	if (pImage)
	{
		UINT16* pImage16 = NULL;
		BOOL bRet = LoadTiffFile(strPath, pImage16, m_nWidth, m_nHeight);
		if (bRet && pImage16)
		{
			int nLoadSize = m_nWidth * m_nHeight;
			if (nImageSize <= nLoadSize)
			{
				for (int iter = 0; iter < nImageSize; iter++)
				{
					pImage[iter] = pImage16[iter];
				}
			}

			delete[] pImage16;
			pImage16 = NULL;
		}
	}

	return bRet;
	//BOOL bRet = FALSE;
	//if (pImage)
	//{
	//	//여기 구현~!!
	//	TIFF* read = TIFFOpen(CT2A(strPath), "r");
	//	if (read)
	//	{
	//		unsigned short *buf = NULL;
	//		buf = (unsigned short*)_TIFFmalloc(TIFFScanlineSize(read));

	//		uint32 imagelength;
	//		uint32 row;
	//		int nWidth, nHeight;
	//		TIFFGetField(read, TIFFTAG_IMAGELENGTH, &imagelength);
	//		TIFFGetField(read, TIFFTAG_IMAGEWIDTH, &nWidth); // set the width of the image
	//		TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nHeight);    // set the height of theimage
	//		//TIFFGetField(read, TIFFTAG_SAMPLESPERPIXEL, &nSamplePerPixel);   // set number of channelsper pixel
	//		//TIFFGetField(read, TIFFTAG_BITSPERSAMPLE, &nBit);    // set the size of the channels
	//		//TIFFGetField(read, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the originof the image.
	//		//TIFFGetField(read, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	//		//TIFFGetField(read, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	//		m_nWidth = nWidth;
	//		m_nHeight = nHeight;
	//		UINT16 *pImage16 = new UINT16[nWidth * nHeight];

	//		for (row = 0; row < imagelength; row++)
	//		{
	//			int nOffset = row * nWidth;
	//			if (TIFFReadScanline(read, buf, row, 0) < 0)
	//				break;
	//			memcpy(&pImage16[nOffset], buf, sizeof(UINT16) * nWidth);
	//		}

	//		for (int nPixel = 0; nPixel < nWidth*nHeight; nPixel++)
	//		{
	//			pImage[nPixel] = pImage16[nPixel];
	//		}

	//		_TIFFfree(buf);
	//		TIFFClose(read);
	//		bRet = TRUE;
	//	}
	//}
	//return bRet;
}

BOOL CImageTiff::LoadTiffFile(const CString& strPath, int*& pImage, int& nWidth, int& nHeight)
{
	UINT16* pImage16 = NULL;
	BOOL bRet = LoadTiffFile(strPath, pImage16, nWidth, nHeight);
	if (bRet && pImage16)
	{
		if (pImage)
		{
			delete[] pImage;
			pImage = NULL;
		}

		int nImageSize = nWidth * nHeight;
		pImage = new int[nImageSize];
		for (int iter = 0; iter < nImageSize; iter++)
		{
			pImage[iter] = pImage16[iter];
		}
	}
	
	return bRet;
}
BOOL CImageTiff::LoadTiffFile(const CString& strPath, UINT16* pImage, int nImageSize)
{
	BOOL bRet = FALSE;
	if (pImage)
	{
		TIFF* read = TIFFOpen(CT2A(strPath), "r");
		if (read)
		{
			int nWidth, nHeight;
			unsigned short* buf = NULL;
			buf = (unsigned short*)_TIFFmalloc(TIFFScanlineSize(read));
			int nImageLength = 0;
			TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nImageLength);
			TIFFGetField(read, TIFFTAG_IMAGEWIDTH, &nWidth); // set the width of the image
			TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nHeight);    // set the height of theimage

			if (nImageSize <= nWidth * nHeight)
			{
				if (m_nWidth != nWidth) m_nWidth = nWidth;
				if (m_nHeight != nHeight) m_nHeight = nHeight;

				for (int row = 0; row < nImageSize; row++)
				{
					int nOffset = row * nWidth;
					if (TIFFReadScanline(read, buf, row, 0) < 0)
						break;
					memcpy(&pImage[nOffset], buf, sizeof(UINT16) * nWidth);
				}

				bRet = TRUE;
			}

			_TIFFfree(buf);
			TIFFClose(read);
		}
	}
	return bRet;
}
BOOL CImageTiff::LoadTiffFile(const CString& strPath, UINT16*& pImage)
{
	return LoadTiffFile(strPath, pImage, m_nWidth, m_nHeight);
}
BOOL CImageTiff::LoadTiffFile(const CString& strPath, UINT16*& pImage, int& nWidth, int& nHeight)
{
	return LoadTiffFile(strPath, pImage, nWidth, nHeight, 1);
}

BOOL CImageTiff::LoadTiffFile(const CString& strPath, UINT16*& pImage, int& nWidth, int& nHeight, int nRGBBit)
{
	BOOL bRet = FALSE;
	TIFF* read = TIFFOpen(CT2A(strPath), "r");
	if (read)
	{
		unsigned short* buf = NULL;
		buf = (unsigned short*)_TIFFmalloc(TIFFScanlineSize(read));
		int nImageSize = 0, nBit = 0;
		TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nImageSize);
		TIFFGetField(read, TIFFTAG_IMAGEWIDTH, &nWidth); // set the width of the image
		TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nHeight);    // set the height of theimage
		TIFFGetField(read, TIFFTAG_BITSPERSAMPLE, &nBit);

		nBit = nBit * nRGBBit;

		if (m_nWidth != nWidth) m_nWidth = nWidth;
		if (m_nHeight != nHeight) m_nHeight = nHeight;

		if (pImage)
		{
			delete[] pImage;
			pImage = NULL;
		}

		pImage = new UINT16[nWidth * nHeight];

		if (nBit == 8)
		{
			BYTE* pImage8bit = new BYTE[nWidth * nHeight];

			for (int row = 0; row < nImageSize; row++)
			{
				int nOffset = row * nWidth;
				if (TIFFReadScanline(read, buf, row, 0) < 0)
					break;
				memcpy(&pImage8bit[nOffset], buf, sizeof(BYTE) * nWidth);
			}

			for (int iter = 0; iter < nWidth * nHeight; iter++)
			{
				pImage[iter] = (UINT16)pImage8bit[iter] << 8;
			}
			delete[] pImage8bit;
			pImage8bit = NULL;
		}
		else if (nBit == 16)
		{
			for (int row = 0; row < nImageSize; row++)
			{
				int nOffset = row * nWidth;
				if (TIFFReadScanline(read, buf, row, 0) < 0)
					break;
				memcpy(&pImage[nOffset], buf, sizeof(UINT16) * nWidth);
			}
		}
		else if (nBit == 24)
		{
			BYTE* pImage8bit = new BYTE[nWidth * nHeight * 3];

			for (int row = 0; row < nImageSize; row++)
			{
				int nOffset = row * nWidth * 3;
				if (TIFFReadScanline(read, buf, row, 0) < 0)
					break;
				memcpy(&pImage8bit[nOffset], buf, sizeof(BYTE) * nWidth * 3);
			}

			for (int iter = 0; iter < nWidth * nHeight; iter++)
			{
				pImage[iter] = (UINT16)pImage8bit[iter * 3] << 8;
			}
			delete[] pImage8bit;
			pImage8bit = NULL;
		}
		else if (nBit == 32)
		{
			UINT32* pImage32bit = new UINT32[nWidth * nHeight];

			for (int row = 0; row < nImageSize; row++)
			{
				int nOffset = row * nWidth;
				if (TIFFReadScanline(read, buf, row, 0) < 0)
					break;
				memcpy(&pImage32bit[nOffset], buf, sizeof(BYTE) * nWidth);
			}

			for (int iter = 0; iter < nWidth * nHeight; iter++)
			{
				pImage[iter] = (UINT16)pImage32bit[iter] >> 16;
			}
			delete[] pImage32bit;
			pImage32bit = NULL;
		}


		_TIFFfree(buf);
		TIFFClose(read);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CImageTiff::LoadTiffFile(const CString& strPath, BYTE*& pImage, int& nWidth, int& nHeight)
{
	BOOL bRet = FALSE;
	TIFF* read = TIFFOpen(CT2A(strPath), "r");
	if (read)
	{
		unsigned short* buf = NULL;
		buf = (unsigned short*)_TIFFmalloc(TIFFScanlineSize(read));
		int nImageSize = 0, nBit = 0;
		TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nImageSize);
		TIFFGetField(read, TIFFTAG_IMAGEWIDTH, &nWidth); // set the width of the image
		TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nHeight);    // set the height of theimage
		

		if (m_nWidth != nWidth) m_nWidth = nWidth;
		if (m_nHeight != nHeight) m_nHeight = nHeight;

		if (pImage)
		{
			delete[] pImage;
			pImage = NULL;
		}

		pImage = new BYTE[nWidth * nHeight];

		for (int row = 0; row < nImageSize; row++)
		{
			int nOffset = row * nWidth;
			if (TIFFReadScanline(read, buf, row, 0) < 0)
				break;
			memcpy(&pImage[nOffset], buf, sizeof(BYTE) * nWidth);
		}

		_TIFFfree(buf);
		TIFFClose(read);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CImageTiff::SaveTiffFile(CString strPath, int* pImage, int nWidth, int nHeight)
{
	BOOL bRet = FALSE;
	if (pImage)
	{
		USES_CONVERSION;

		TIFF *out = TIFFOpen(CT2A(strPath), "w");
		if (out)
		{
			int sampleperpixel = 1;

			TIFFSetField(out, TIFFTAG_IMAGEWIDTH, nWidth); // set the width of the image
			TIFFSetField(out, TIFFTAG_IMAGELENGTH, nHeight);    // set the height of theimage
			TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);   // set number of channelsper pixel
			TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 16);    // set the size of the channels
			TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the originof the image.
			//   Some other essential fields to set thatyou do not have to understand for now.
			TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

			unsigned short *buf = NULL;        // bufferused to store the row of pixel information for writing tofile
			buf = (unsigned short*)_TIFFmalloc(TIFFScanlineSize(out));

			// We set the strip size of the file to be size of onerow of pixels
			TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, nWidth*sampleperpixel));

			//Now writing image to the file one strip at atime
			UINT16* pImg16 = new UINT16[nWidth*nHeight];
			for (int nPixel = 0; nPixel < nWidth*nHeight; nPixel++)
			{
				if (pImage[nPixel] > 65535)
				{
					pImg16[nPixel] = 65535;
				}
				else if(pImage[nPixel] < 0)
				{
					pImg16[nPixel] = 0;
				}
				else
				{
					pImg16[nPixel] = pImage[nPixel];
				}
				
			}

			for (int row = 0; row < nHeight; row++)
			{
				int nOffset = row * nWidth;
				memcpy(buf, &pImg16[nOffset], sizeof(UINT16) * nWidth);
				if (TIFFWriteScanline(out, buf, row, 0) < 0)
					break;
			}

			delete[] pImg16;
			pImg16 = NULL;

			TIFFClose(out);

			if (buf)
			{
				_TIFFfree(buf);
			}
			bRet = TRUE;
		}
	}
	return bRet;
}

BOOL CImageTiff::SaveTiffFile(CString strPath, UINT16* pImage, int nWidth, int nHeight)
{
	BOOL bRet = FALSE;
	if (pImage)
	{
		USES_CONVERSION;

		TIFF* out = TIFFOpen(CT2A(strPath), "w");
		if (out)
		{
			int sampleperpixel = 1;

			TIFFSetField(out, TIFFTAG_IMAGEWIDTH, nWidth); // set the width of the image
			TIFFSetField(out, TIFFTAG_IMAGELENGTH, nHeight);    // set the height of theimage
			TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);   // set number of channelsper pixel
			TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 16);    // set the size of the channels
			TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the originof the image.
			//   Some other essential fields to set thatyou do not have to understand for now.
			TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

			unsigned short* buf = NULL;        // bufferused to store the row of pixel information for writing tofile
			buf = (unsigned short*)_TIFFmalloc(TIFFScanlineSize(out));

			// We set the strip size of the file to be size of onerow of pixels
			TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, nWidth * sampleperpixel));

			//Now writing image to the file one strip at atime
			//UINT16* pImg16 = new UINT16[nWidth * nHeight];
			//for (int nPixel = 0; nPixel < nWidth * nHeight; nPixel++)
			//{
			//	pImg16[nPixel] = pImage[nPixel];
			//}

			for (int row = 0; row < nHeight; row++)
			{
				int nOffset = row * nWidth;
				memcpy(buf, &pImage[nOffset], sizeof(UINT16) * nWidth);
				if (TIFFWriteScanline(out, buf, row, 0) < 0)
					break;
			}

			TIFFClose(out);

			if (buf)
			{
				_TIFFfree(buf);
			}
			bRet = TRUE;
		}
	}
	return bRet;
}
BOOL CImageTiff::SaveTiffFile(CString strPath, float* pImage, int nWidth, int nHeight)
{
	BOOL bRet = FALSE;
	if (pImage)
	{
		USES_CONVERSION;

		TIFF* out = TIFFOpen(CT2A(strPath), "w");
		if (out)
		{

			int16 nSamplePerPixel = 1;
			int16 nBitsPerSample = 32;

			TIFFSetField(out, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
			TIFFSetField(out, TIFFTAG_IMAGEWIDTH, nWidth); // set the width of the image
			TIFFSetField(out, TIFFTAG_IMAGELENGTH, nHeight);    // set the height of theimage
			TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, nSamplePerPixel);   // set number of channelsper pixel
			TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, nBitsPerSample);    // set the size of the channels
			TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the originof the image.
			//   Some other essential fields to set thatyou do not have to understand for now.
			TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

			float* buf = NULL;        // bufferused to store the row of pixel information for writing tofile
			buf = (float*)_TIFFmalloc(TIFFScanlineSize(out));

			// We set the strip size of the file to be size of onerow of pixels
			TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, nWidth * nSamplePerPixel));

			//Now writing image to the file one strip at atime
			for (int row = 0; row < nHeight; row++)
			{
				int nOffset = row * nWidth;
				memcpy(buf, &pImage[nOffset], sizeof(float) * nWidth);
				if (TIFFWriteScanline(out, buf, row, 0) < 0)
					break;
			}

			TIFFClose(out);

			if (buf)
			{
				_TIFFfree(buf);
			}
			bRet = TRUE;
		}
	}
	return bRet;
}
BOOL CImageTiff::SaveTiffFile(CString strPath, unsigned char* pImage, int nWidth, int nHeight)
{
	BOOL bRet = FALSE;
	if (pImage)
	{
		USES_CONVERSION;

		TIFF* out = TIFFOpen(CT2A(strPath), "w");
		if (out)
		{
			int16 nSamplePerPixel = 1;
			int16 nBitsPerSample = 8;
			TIFFSetField(out, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
			TIFFSetField(out, TIFFTAG_IMAGEWIDTH, nWidth); // set the width of the image
			TIFFSetField(out, TIFFTAG_IMAGELENGTH, nHeight);    // set the height of theimage
			TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, nSamplePerPixel);   // set number of channelsper pixel
			TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, nBitsPerSample);    // set the size of the channels
			TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the originof the image.
			//   Some other essential fields to set thatyou do not have to understand for now.
			TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

			unsigned char* buf = NULL;        // bufferused to store the row of pixel information for writing tofile
			buf = (unsigned char*)_TIFFmalloc(TIFFScanlineSize(out));

			// We set the strip size of the file to be size of onerow of pixels
			TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, nWidth * nSamplePerPixel));

			//Now writing image to the file one strip at atime
			for (int row = 0; row < nHeight; row++)
			{
				int nOffset = row * nWidth;
				memcpy(buf, &pImage[nOffset], sizeof(unsigned char) * nWidth);
				if (TIFFWriteScanline(out, buf, row, 0) < 0)
					break;
			}

			TIFFClose(out);

			if (buf)
			{
				_TIFFfree(buf);
			}
			bRet = TRUE;
		}
	}
	return bRet;
}
