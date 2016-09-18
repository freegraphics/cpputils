#if !defined(__MEMORYDC_H__9E73516A_AA98_4934_AAFE_7B3D7DF91B0F__)
#define __MEMORYDC_H__9E73516A_AA98_4934_AAFE_7B3D7DF91B0F__

#define IMAGESIZE    (-1)

#include <math.h>
#include <utils/utils.h>
#include <math/utils.h>
#include <vfw.h>

#pragma comment(lib , "Vfw32.lib")


#if !defined(M_PI)
#define M_PI 3.1415926535897932384626433832795
#endif

inline
double RGBtoGray(BYTE _r,BYTE _g,BYTE _b)
{
	return _r*0.2989+_g*0.5870+_b*0.1140;
}

inline
double RGBtoGray(BYTE _r,BYTE _g,BYTE _b,BYTE _a)
{
	double af = _a/256.0;
	return _r*0.2989*af+_g*0.5870*af+_b*0.1140*af;
}

class MemoryDC
{
protected:
	HDC m_hdc;
	HPALETTE m_hpal;
	HBITMAP m_hbmp;

	HPALETTE m_holdpal;
	HBITMAP m_holdbmp;
	LPVOID m_lpBits;
	BYTE** m_lines;
	DWORD m_widthbytes;
	DWORD m_height;
	WORD m_bitspixel;
	bool m_bfree,m_bpal;
	LPBITMAPINFO m_bih;

public:
	MemoryDC() 
		:m_hdc(NULL),m_hpal(NULL),m_hbmp(NULL),m_holdpal(NULL),
		m_holdbmp(NULL),m_lines(NULL),m_lpBits(NULL),m_bih(NULL) 
	{
		m_bfree=true;
	};

	~MemoryDC()	
	{
		free();
	};

	bool isEmpty() const {return m_hdc==NULL;}
	HDC getDC() const {return m_hdc;}
	operator HBITMAP() {detachBMP();return m_hbmp;}
	operator HPALETTE() const {return m_hpal;}
	LPVOID getBits() const {return m_lpBits;}

	void fill(BYTE dta);
	LPBITMAPINFO getBMI() {return m_bih;}
	const LPBITMAPINFO getBMI() const {return m_bih;}
	void GetBmpInfo(LPBITMAP pbmpinf);
	void detachBMP();
	void reattachBMP();
	void free();
	void Create(const CSize& _imgsize,int _bitcnt,bool _bMonohrome=true,int _clrcnt=0,PLOGPALETTE _ppal=NULL);
	void Attach(HBITMAP hbmp,PBITMAPINFO pBIH,LPVOID _lpBits,HPALETTE hPal = NULL,PPALETTEENTRY ppal=NULL);
	BYTE** getLinesArray();

	DWORD getWidthBytes()	{return m_widthbytes;}
	LONG getHeight() const {return m_height;}
	LONG getWidth() const {return m_bih->bmiHeader.biWidth;}
	void draw(CDC& dc,HDRAWDIB hdd,const CRect& _destrc,const CRect* _srcrc=NULL) const;
	void Save(LPCTSTR szFileName);
	bool Load(LPCTSTR szFileName);
	bool Load(HINSTANCE _hInst,UINT _nImgID,UINT fuLoad = 0);
	void CreateMonoPalette(int clrcnt=256);
	void GetRGBA(long _x,long _y,BYTE& _r,BYTE& _g,BYTE& _b,BYTE& _alpha) const;
	void SetRGBA(long _x,long _y,BYTE _r,BYTE _g,BYTE _b,BYTE _alpha = 255);
	void SetRGBA(long _x,long _y,COLORREF _clr);
	void SetGray(long _x,long _y,BYTE _g);
	void FillRGB(COLORREF _clr);
	LPBYTE get_PackedData() const;
	bool Load(HBITMAP hbmp);
};


inline
void MemoryDC::Create(const CSize& _imgsize,int _bitcnt,bool _bMonohrome/*=true*/,int _clrcnt/*=0*/,PLOGPALETTE _ppal/*=NULL*/)
{
	int cnt,i,j;
	HDC hdc;
	LPBITMAPINFO pbmpi;
	//LPVOID lpBits;
	free();

	if(_ppal!=NULL) _clrcnt = _ppal->palNumEntries;

	VERIFY_EXIT(_bitcnt==1 || _bitcnt==8 || _bitcnt==16 || _bitcnt==24 || _bitcnt==32);
	m_bitspixel = _bitcnt;
	if(!_clrcnt && _bitcnt<=8) _clrcnt = 1<<_bitcnt;
	else if(!_ppal) _clrcnt = 0;
	cnt = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*_clrcnt;
	pbmpi = (PBITMAPINFO)new BYTE[cnt];
	ZeroMemory(pbmpi,cnt);
	pbmpi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	hdc = ::GetDC(::GetDesktopWindow());

	pbmpi->bmiHeader.biWidth = _imgsize.cx;
	pbmpi->bmiHeader.biHeight = _imgsize.cy;
	pbmpi->bmiHeader.biCompression = BI_RGB;
	pbmpi->bmiHeader.biBitCount = _bitcnt;
	pbmpi->bmiHeader.biPlanes = 1;
	pbmpi->bmiHeader.biXPelsPerMeter = 0;
	pbmpi->bmiHeader.biYPelsPerMeter = 0;
	//pbmpi->bmiHeader.biSizeImage = 
	//	pbmpi->bmiHeader.biWidth*pbmpi->bmiHeader.biHeight

	if(_bitcnt==8 || _bitcnt==1)
	{
		pbmpi->bmiHeader.biClrUsed = _clrcnt;
		pbmpi->bmiHeader.biClrImportant = _clrcnt;
	}
	else 
	{
		pbmpi->bmiHeader.biClrUsed = 0;
		pbmpi->bmiHeader.biClrImportant = 0;
	}

	m_hdc = ::CreateCompatibleDC(hdc);

	if(_bitcnt<16 || _ppal)
	{
		cnt = sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*_clrcnt;
		PLOGPALETTE pPalette = (PLOGPALETTE)new BYTE[cnt];
		ZeroMemory(pPalette,cnt);
		pPalette->palVersion = 0x300;
		pPalette->palNumEntries = _clrcnt;
		if(_ppal!=NULL)
		{
			for(i=0;i<_clrcnt;i++)
			{
				pPalette->palPalEntry[i] = _ppal->palPalEntry[i];

				pbmpi->bmiColors[i].rgbRed = _ppal->palPalEntry[i].peRed;
				pbmpi->bmiColors[i].rgbGreen = _ppal->palPalEntry[i].peGreen;
				pbmpi->bmiColors[i].rgbBlue = _ppal->palPalEntry[i].peBlue;
				pbmpi->bmiColors[i].rgbReserved = 0;
			}
		}
		else if(_bMonohrome)
		{
			//ok here build monochrome palette
			for(i=0;i<_clrcnt;i++)
			{
				BYTE clr = 255*i/(_clrcnt-1);
				pPalette->palPalEntry[i].peRed = clr;
				pPalette->palPalEntry[i].peGreen = clr;
				pPalette->palPalEntry[i].peBlue = clr;
				pPalette->palPalEntry[i].peFlags = 0;

				pbmpi->bmiColors[i].rgbRed = clr;
				pbmpi->bmiColors[i].rgbGreen = clr;
				pbmpi->bmiColors[i].rgbBlue = clr;
				pbmpi->bmiColors[i].rgbReserved = 0;
			}
		}
		else
		{

			VERIFY_EXIT(_clrcnt==256);
			for(i=0;i<16;i++)
			{
				BYTE clr = (255*i)/15;
				pPalette->palPalEntry[i].peRed = clr;
				pPalette->palPalEntry[i].peGreen = clr;
				pPalette->palPalEntry[i].peBlue = clr;
				pPalette->palPalEntry[i].peFlags = 0;
			}
			double p = 1/20.0;
			for(i=0;i<60;i++)
			{
				double r=0,g=0,b=0;
				BYTE maxclr = 255;
				if(i<20)
				{
					r = cos(M_PI*i*p);
					g = cos(M_PI*(i-20)*p);

					r = sign(r)*cube(fabs(r))+1;
					g = sign(g)*cube(fabs(g))+1;
				}
				else if(i<40)
				{
					g = cos(M_PI*(i-20)*p);
					b = cos(M_PI*(i-40)*p);

					g = sign(g)*cube(fabs(g))+1;
					b = sign(b)*cube(fabs(b))+1;
				}
				else
				{
					b = cos(M_PI*(i-40)*p);
					r = cos(M_PI*(i-60)*p);

					b = sign(b)*cube(fabs(b))+1;
					r = sign(r)*cube(fabs(r))+1;
				}
				double mcomp = max_value(r,max_value(g,b));
				r/=mcomp;g/=mcomp;b/=mcomp;
				int index = 16+i;
				for(j=1;j<5;j++,index+=60)
				{
					pPalette->palPalEntry[index].peRed = (BYTE)(r*j*0.25*maxclr);
					pPalette->palPalEntry[index].peGreen = (BYTE)(g*j*0.25*maxclr);
					pPalette->palPalEntry[index].peBlue = (BYTE)(b*j*0.25*maxclr);
					pPalette->palPalEntry[index].peFlags = 0;
				}
			}
			for(i=0;i<256;i++)
			{
				pbmpi->bmiColors[i].rgbRed = pPalette->palPalEntry[i].peRed;
				pbmpi->bmiColors[i].rgbGreen = pPalette->palPalEntry[i].peGreen;
				pbmpi->bmiColors[i].rgbBlue = pPalette->palPalEntry[i].peBlue;
				pbmpi->bmiColors[i].rgbReserved = 0;
			}
		}
		m_hpal = ::CreatePalette(pPalette);
		m_bpal = true;
		delete[] pPalette;
		m_holdpal = ::SelectPalette(m_hdc,m_hpal,TRUE);
	}

	m_hbmp = ::CreateDIBSection(m_hdc,pbmpi,DIB_RGB_COLORS,&m_lpBits,NULL,0);
	::SaveDC(m_hdc);
	m_holdbmp = (HBITMAP)::SelectObject(m_hdc,m_hbmp);
	::SetMapMode(m_hdc,MM_TEXT);
	m_bih = pbmpi;
	//delete[] pbmpi;
	m_bfree = true;

	m_widthbytes = m_bih->bmiHeader.biWidth*
		m_bih->bmiHeader.biBitCount/8;
	if(m_bih->bmiHeader.biBitCount==1 
		&& m_bih->bmiHeader.biWidth) ++m_widthbytes;
	m_widthbytes = (m_widthbytes+3)& ~3;
	m_height = m_bih->bmiHeader.biHeight;
}

inline
void MemoryDC::Attach(HBITMAP hbmp,PBITMAPINFO pBIH,LPVOID _lpBits,HPALETTE hPal /*= NULL*/,PPALETTEENTRY ppal/*=NULL*/)
{
	HDC hdc;
	int clrcnt,i;
	hdc = ::GetDC(::GetDesktopWindow());
	m_hdc = ::CreateCompatibleDC(hdc);
	if(hPal==NULL)
	{
		clrcnt = pBIH->bmiHeader.biClrUsed;
		if(clrcnt==0) clrcnt = 1<<pBIH->bmiHeader.biBitCount;
		if(clrcnt<=255)
		{
			PLOGPALETTE pPalette = (PLOGPALETTE)
				new BYTE[(clrcnt-1)*sizeof(PALETTEENTRY)+sizeof(LOGPALETTE)];
			pPalette->palNumEntries = clrcnt;
			pPalette->palVersion = 0x300;
			if(ppal!=NULL)
			{
				for(i=0;i<clrcnt;i++)
				{
					pPalette->palPalEntry[i] = ppal[i];
					pPalette->palPalEntry[i].peFlags = 0;
				}
			}
			else
			{
				for(i=0;i<clrcnt;i++)
				{
					pPalette->palPalEntry[i].peRed =  pBIH->bmiColors[i].rgbRed;
					pPalette->palPalEntry[i].peGreen = pBIH->bmiColors[i].rgbGreen;
					pPalette->palPalEntry[i].peBlue = pBIH->bmiColors[i].rgbBlue;
					pPalette->palPalEntry[i].peFlags = 0;
				}
			}
			hPal = ::CreatePalette(pPalette);
			delete[] pPalette;
			m_bpal = true;
		}
	}
	clrcnt = 1<<pBIH->bmiHeader.biBitCount;
	int cnt = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*clrcnt;
	PBITMAPINFO pbmpi = (PBITMAPINFO)new BYTE[cnt];
	memcpy(pbmpi,pBIH,cnt);

	m_hpal = hPal;
	m_holdpal = (HPALETTE)::SelectObject(m_hdc,m_hpal);
	m_bitspixel = pBIH->bmiHeader.biBitCount;
	m_hbmp = hbmp;
	m_holdbmp = (HBITMAP)::SelectObject(m_hdc,m_hbmp);
	m_lpBits = _lpBits;
	::SetMapMode(m_hdc,MM_TEXT);
	m_bfree = false;
	m_bih = pbmpi;

	m_widthbytes = m_bih->bmiHeader.biWidth*
		m_bih->bmiHeader.biBitCount/8;
	if(m_bih->bmiHeader.biBitCount==1 
		&& m_bih->bmiHeader.biWidth) ++m_widthbytes;
	m_widthbytes = (m_widthbytes+3)& ~3;
	m_height = m_bih->bmiHeader.biHeight;
}

inline
void MemoryDC::Save(LPCTSTR szFileName)
{
	m_bih->bmiHeader.biSizeImage = m_widthbytes*m_height;
	::GdiFlush();

	BITMAPFILEHEADER hdr;
	hdr.bfType = 0x4d42;
	hdr.bfOffBits = sizeof(BITMAPFILEHEADER)
		+ m_bih->bmiHeader.biSize 
		+ m_bih->bmiHeader.biClrUsed*sizeof(RGBQUAD);
	hdr.bfSize = (DWORD)(hdr.bfOffBits
		+ m_bih->bmiHeader.biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	HANDLE hFile;
	hFile = CreateFile(szFileName,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL);
	if(hFile==INVALID_HANDLE_VALUE) return;

	DWORD dwtmp,sz;
	sz = sizeof(BITMAPFILEHEADER);
	if(!WriteFile(hFile,&hdr,sz,&dwtmp,0) && dwtmp!=sz)
		goto err_exit;

	sz = m_bih->bmiHeader.biSize 
		+ m_bih->bmiHeader.biClrUsed*sizeof(RGBQUAD);
	if(!WriteFile(hFile,m_bih,sz,&dwtmp,NULL) && dwtmp!=sz)
		goto err_exit;

	sz = m_bih->bmiHeader.biSizeImage;
	if(!WriteFile(hFile,m_lpBits,sz,&dwtmp,NULL) && dwtmp!=sz)
		goto err_exit;
err_exit:
	CloseHandle(hFile);
}

inline
void MemoryDC::CreateMonoPalette(int clrcnt/*=256*/)
{
	DWORD sz = sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*clrcnt;
	PLOGPALETTE pPalette = (PLOGPALETTE)new BYTE[sz];
	ZeroMemory(pPalette,sz);
	pPalette->palVersion = 0x300;
	pPalette->palNumEntries = clrcnt;
	int i;
	for(i=0;i<clrcnt;i++)
	{
		BYTE clr = 255*i/(clrcnt-1);
		pPalette->palPalEntry[i].peRed = clr;
		pPalette->palPalEntry[i].peGreen = clr;
		pPalette->palPalEntry[i].peBlue = clr;
		pPalette->palPalEntry[i].peFlags = 0;
	}
	if(m_hpal!=NULL)
	{
		if(m_holdpal) ::SelectPalette(m_hdc,m_holdpal,TRUE);
		if(m_bpal && m_hpal) ::DeleteObject(m_hpal);
	}
	m_hpal = ::CreatePalette(pPalette);
	m_bpal = true;
	delete[] pPalette;
	m_holdpal = ::SelectPalette(m_hdc,m_hpal,TRUE);
}

inline
void MemoryDC::GetRGBA(long _x,long _y,BYTE& _r,BYTE& _g,BYTE& _b,BYTE& _alpha) const
{
	const BYTE* pdta = (const BYTE*)getBits();
	switch (m_bih->bmiHeader.biBitCount)
	{
	case 24:
		{
			const BYTE* pPixel = pdta + (m_height-_y-1)*m_widthbytes + _x*3;
			_r = pPixel[0];
			_g = pPixel[1];
			_b = pPixel[2];
			_alpha = 255;
			break;
		}
	case 32:
		{
			const BYTE* pPixel = pdta + (m_height-_y-1)*m_widthbytes + _x*4;
			_r = pPixel[0];
			_g = pPixel[1];
			_b = pPixel[2];
			_alpha = pPixel[3];
			break;
		}
	case 8:
		{
			const BYTE* pPixel = pdta + (m_height-_y-1)*m_widthbytes + _x;
			_r = pPixel[0];
			_g = pPixel[0];
			_b = pPixel[0];
			_alpha = 255;
			break;
		}
	case 1:
	case 16:
	default:
		VERIFY_EXIT(FALSE);
		break;
	}
}

inline
void MemoryDC::SetRGBA(long _x,long _y,BYTE _r,BYTE _g,BYTE _b,BYTE _alpha /*= 255*/)
{
	LPBYTE pdta = (LPBYTE)getBits();
	switch (m_bih->bmiHeader.biBitCount)
	{
	case 24:
		{
			BYTE* pPixel = ((LPBYTE)m_lpBits) + (m_height-_y-1)*m_widthbytes + _x*3;
			pPixel[0] = _r;
			pPixel[1] = _g;
			pPixel[2] = _b;
			break;
		}
	case 32:
		{
			BYTE* pPixel = ((LPBYTE)m_lpBits) + (m_height-_y-1)*m_widthbytes + _x*4;
			pPixel[0] = _r;
			pPixel[1] = _g;
			pPixel[2] = _b;
			pPixel[3] = _alpha;
			break;
		}
	case 8:
		{
			BYTE* pPixel = ((LPBYTE)m_lpBits) + (m_height-_y-1)*m_widthbytes + _x;
			pPixel[0] = (BYTE)RGBtoGray(_r,_g,_b);
			break;
		}
	}
}

inline
void MemoryDC::SetGray(long _x,long _y,BYTE _g)
{
	LPBYTE pdta = (LPBYTE)getBits();
	switch (m_bih->bmiHeader.biBitCount)
	{
	case 8:
		{
			BYTE* pPixel = ((LPBYTE)m_lpBits) + ((m_height-_y-1)*m_widthbytes + _x);
			pPixel[0] = _g;
			break;
		}
	default:
		SetRGBA(_x,_y,_g,_g,_g,255);
		break;
	}
}

inline
LPBYTE MemoryDC::get_PackedData() const
{
	::GdiFlush();

	LPBYTE pdta = (LPBYTE)getBits();
	LPBYTE packed = NULL;
	switch (m_bih->bmiHeader.biBitCount)
	{
	case 8:
		{
			packed = new BYTE[getWidth()*getHeight()*3];
			long y=0,x=0,j=0;
			for(y=0,j=0;y<getHeight();y++)
				for(x=0;x<getWidth();x++)
				{
					packed[j++] = ((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x];
					packed[j++] = ((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x];
					packed[j++] = ((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x];
				}
		}
		break;
	case 24:
		{
			packed = new BYTE[getWidth()*getHeight()*3];
			long y=0,x=0,j=0;
			for(y=0,j=0;y<getHeight();y++)
				for(x=0;x<getWidth();x++)
				{
					packed[j++] = ((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x*3];
					packed[j++] = ((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x*3+1];
					packed[j++] = ((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x*3+2];
				}
		}
		break;
	}
	return packed;
}

inline
void MemoryDC::fill(BYTE dta)
{
	if(m_hbmp) return;

	BITMAP bmpinfo;
	GetBmpInfo(&bmpinfo);
	DWORD widthbytes,height;
	widthbytes = bmpinfo.bmWidth*bmpinfo.bmBitsPixel/8;
	if(bmpinfo.bmBitsPixel==1 && bmpinfo.bmWidth%8) ++widthbytes;
	widthbytes = (widthbytes+3)& ~3;
	height = bmpinfo.bmHeight;
	LPBYTE pdta = (LPBYTE)getBits();
	FillMemory(pdta,widthbytes*height,dta);
}

inline
void MemoryDC::GetBmpInfo(LPBITMAP pbmpinf)
{
	if(!m_hbmp)
	{
		ZeroMemory(pbmpinf,sizeof(BITMAP));
		return; 
	}
	::GetObject(m_hbmp,sizeof(BITMAP),pbmpinf);
}

inline
void MemoryDC::detachBMP()
{
	VERIFY_EXIT(NOT_NULL(m_hdc));
	if(!m_holdbmp) return;
	::GdiFlush();
	::RestoreDC(m_hdc,-1);
	::SaveDC(m_hdc);
	::SelectObject(m_hdc,m_holdbmp);
	m_holdbmp = NULL;
}

inline
void MemoryDC::reattachBMP()
{
	VERIFY_EXIT(NOT_NULL(m_hdc) && NOT_NULL(m_hbmp));
	if(m_holdbmp ) return;
	::RestoreDC(m_hdc,-1);
	::SaveDC(m_hdc);
	m_holdbmp = (HBITMAP)::SelectObject(m_hdc,m_hbmp);
}

inline
void MemoryDC::free()
{
	if(!m_hdc) return;
	if(m_holdpal) ::SelectPalette(m_hdc,m_holdpal,TRUE);
	if(m_holdbmp) ::SelectObject(m_hdc,m_holdbmp);
	::RestoreDC(m_hdc,-1);
	::DeleteObject(m_hdc);
	if(m_bfree && m_hbmp)	::DeleteObject(m_hbmp);
	if(m_bpal && m_hpal) ::DeleteObject(m_hpal);
	if(m_lines) delete[] m_lines;
	if(m_bih) delete[] m_bih;

	m_hdc = NULL;
	m_hbmp = NULL;
	m_hpal = NULL;
	m_holdpal = NULL;
	m_holdbmp = NULL;
	m_lines = NULL;
	m_bih = NULL;
}

inline
BYTE** MemoryDC::getLinesArray()
{
	if(!m_lines)
	{
		//BITMAP bmpinfo;
		//::GetObject(m_hbmp,sizeof(BITMAP),&bmpinfo);
		//m_widthbytes = bmpinfo.bmWidth*bmpinfo.bmBitsPixel/8;
		//if(bmpinfo.bmBitsPixel==1 && bmpinfo.bmWidth%8) ++m_widthbytes;
		//m_widthbytes = (m_widthbytes+3)& ~3;
		//m_height = bmpinfo.bmHeight;
		m_lines = new BYTE* [m_height];
		//if()
		for(int y=0;y<(int)m_height;y++)
			m_lines[y] = (LPBYTE)m_lpBits + (m_height-y-1)*m_widthbytes;
	}
	return m_lines;
}

inline
bool MemoryDC::Load(LPCTSTR szFileName)
{
	return Load((HBITMAP)::LoadImage(NULL,szFileName,
		IMAGE_BITMAP,0,0,
		LR_CREATEDIBSECTION|LR_LOADFROMFILE)
		);
}

inline
bool MemoryDC::Load(HINSTANCE _hInst,UINT _nImgID,UINT fuLoad /*= 0*/)
{
	fuLoad &= ~LR_LOADFROMFILE;
	return Load((HBITMAP)::LoadImage(_hInst,MAKEINTRESOURCE(_nImgID)
		,IMAGE_BITMAP,0,0
		,LR_CREATEDIBSECTION|fuLoad)
		);
}

inline
bool MemoryDC::Load(HBITMAP _hbmp)
{
	free();
	m_hbmp = _hbmp;
	if(m_hbmp==NULL) return false;
	DIBSECTION dibs;
	GetObject(m_hbmp,sizeof(dibs),&dibs);
	DWORD sz;
	sz = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*dibs.dsBmih.biClrUsed;
	m_bih = (LPBITMAPINFO)new BYTE[sz];
	*((LPBITMAPINFOHEADER)m_bih) = dibs.dsBmih;
	m_lpBits = dibs.dsBm.bmBits;
	m_bitspixel = m_bih->bmiHeader.biBitCount;
	int i;
	HDC hdc;
	hdc = ::GetDC(::GetDesktopWindow());
	m_hdc = ::CreateCompatibleDC(hdc);
	::SaveDC(m_hdc);
	m_holdbmp = (HBITMAP)::SelectObject(m_hdc,m_hbmp);
	::SetMapMode(m_hdc,MM_TEXT);
	DWORD& clrcnt = m_bih->bmiHeader.biClrUsed;
	if(clrcnt>0)
	{
		sz = sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*clrcnt;
		PLOGPALETTE pPalette = (PLOGPALETTE)new BYTE[sz];
		pPalette->palNumEntries = (WORD)clrcnt;
		pPalette->palVersion = 0x0300;
		// retrieve color table info 
		GetDIBColorTable(m_hdc,0,clrcnt,m_bih->bmiColors);
		LPRGBQUAD clrs=m_bih->bmiColors;
		LPPALETTEENTRY ppal=pPalette->palPalEntry;
		for(i=0;(DWORD)i<clrcnt;i++,ppal++,clrs++)
		{	
			ppal->peRed=clrs->rgbRed;
			ppal->peGreen = clrs->rgbGreen;
			ppal->peBlue = clrs->rgbBlue;
			ppal->peFlags = 0;
		}
		m_hpal = ::CreatePalette(pPalette);
		m_bpal = true;
		delete[] pPalette;
		m_holdpal = ::SelectPalette(m_hdc,m_hpal,TRUE);
	}
	m_bfree = true;

	m_widthbytes = m_bih->bmiHeader.biWidth*
		m_bih->bmiHeader.biBitCount/8;
	if(m_bih->bmiHeader.biBitCount==1 
		&& m_bih->bmiHeader.biWidth) ++m_widthbytes;
	m_widthbytes = (m_widthbytes+3)& ~3;
	m_height = m_bih->bmiHeader.biHeight;
	return true;
}

inline
void MemoryDC::draw(CDC& dc,HDRAWDIB hdd,const CRect& _destrc,const CRect* _psrcrc/*=NULL*/) const
{
	if(isEmpty()) return;
	CRect srcrc;
	if(_psrcrc!=NULL) srcrc = *_psrcrc;
	else srcrc.SetRect(0,0,m_bih->bmiHeader.biWidth,abs(m_bih->bmiHeader.biHeight));

	if(hdd)
		::DrawDibDraw(hdd,dc.GetSafeHdc()
			,_destrc.left,_destrc.top,_destrc.Width(),_destrc.Height()
			,&m_bih->bmiHeader,getBits()
			,srcrc.left,srcrc.top,srcrc.Width(),srcrc.Height()
			,DDF_HALFTONE
			);
	else ::StretchDIBits(dc.GetSafeHdc()
			,_destrc.left,_destrc.top,_destrc.Width(),_destrc.Height()
			,srcrc.left,srcrc.top,srcrc.Width(),srcrc.Height()
			,getBits(),m_bih,DIB_RGB_COLORS,SRCCOPY
			);
}

inline
void MemoryDC::SetRGBA(long _x,long _y,COLORREF _clr)
{
	SetRGBA(_x,_y,GetRValue(_clr),GetGValue(_clr),GetBValue(_clr));
}

inline
void MemoryDC::FillRGB(COLORREF _clr)
{
	::GdiFlush();

	BYTE r = GetRValue(_clr);
	BYTE g = GetGValue(_clr);
	BYTE b = GetBValue(_clr);

	LPBYTE pdta = (LPBYTE)getBits();
	switch (m_bih->bmiHeader.biBitCount)
	{
	case 8:
		{
			long y=0,x=0,j=0;
			for(y=0,j=0;y<getHeight();y++)
				for(x=0;x<getWidth();x++)
				{
					((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x] = r;
				}
		}
		break;
	case 24:
		{
			long y=0,x=0,j=0;
			for(y=0,j=0;y<getHeight();y++)
				for(x=0;x<getWidth();x++)
				{
					((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x*3] = r;
					((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x*3+1] = g;
					((LPBYTE)m_lpBits)[(m_height-y-1)*m_widthbytes + x*3+2] = b;
				}
		}
		break;
	}
}

#endif//#if !defined(__MEMORYDC_H__9E73516A_AA98_4934_AAFE_7B3D7DF91B0F__)