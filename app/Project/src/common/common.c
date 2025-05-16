/*********************************************************************************************************
*
* File                : common.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.74
* Version             : V1.0
* Author              : 
* Time                :
* Brief               : 
*********************************************************************************************************/
#include "includes.h"


const uint32_t CRC32_tab[] =
{
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419,
	0x706AF48F, 0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4,
	0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07,
	0x90BF1D91, 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
	0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x136C9856,
	0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
	0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4,
	0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3,
	0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 0x26D930AC, 0x51DE003A,
	0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599,
	0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190,
	0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F,
	0x9FBFE4A5, 0xE8B8D433, 0x7807C9A2, 0x0F00F934, 0x9609A88E,
	0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED,
	0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
	0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3,
	0xFBD44C65, 0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
	0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A,
	0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5,
	0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA, 0xBE0B1010,
	0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17,
	0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6,
	0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615,
	0x73DC1683, 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
	0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1, 0xF00F9344,
	0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
	0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A,
	0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1,
	0xA6BC5767, 0x3FB506DD, 0x48B2364B, 0xD80D2BDA, 0xAF0A1B4C,
	0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF,
	0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE,
	0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31,
	0x2CD99E8B, 0x5BDEAE1D, 0x9B64C2B0, 0xEC63F226, 0x756AA39C,
	0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B,
	0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
	0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1,
	0x18B74777, 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
	0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45, 0xA00AE278,
	0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7,
	0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66,
	0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605,
	0xCDD70693, 0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8,
	0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B,
	0x2D02EF8D
};

unsigned int Cal_Crc32(unsigned int init,const unsigned char *data, unsigned int len, unsigned char status)
{
		unsigned int ret = init;
		
		int i=0;
		
		for(i=0; i<len; i++)
		{
			ret = CRC32_tab[(int)((ret ^ data[i]) & 0xff)] ^ ((ret >> 8) & 0xffffff);
		}
		
    if(status)
    {
        ret ^= 0xffffffff;
        ret &= 0x00000000FFFFFFFFL;
    }

		return ret;
}

unsigned short GetCrc16(unsigned short usCrcInit, unsigned char *pData, int iLen)
{
	unsigned char b = 0;
	unsigned short crc = usCrcInit;
	int i, j;
	for(i=0; i<iLen; i++)
	{        
		for(j=0; j<8; j++)
		{
			b = ((pData[i]<<j)&0x80)^((crc&0x8000)>>8);
			crc<<=1;
			if(b!=0)
			{
				crc^=0x1021;
			}
		}
	}
	return crc;
}



int ChangeHexStringToHexArray(char *pSrc, unsigned short usLen, char *pDst)
{
	unsigned short i=0,j =0;
	unsigned char ucTmp = 0;
	
	for(i=0; i<usLen; i++)
	{
		if((pSrc[2*i]>='0')&&(pSrc[2*i]<='9'))
		{
			ucTmp = pSrc[2*i] - '0';
		}
		else if((pSrc[2*i]>='a')&&(pSrc[2*i]<='f'))
		{
			ucTmp = pSrc[2*i] - 'a' + 0x0A;
		}
		else if((pSrc[2*i]>='A')&&(pSrc[2*i]<='F'))
		{
			ucTmp = pSrc[2*i] - 'A' + 0x0A;
		}
		else
		{
			return -1;
		}
		ucTmp <<= 4;
		
		if((pSrc[2*i+1]>='0')&&(pSrc[2*i+1]<='9'))
		{
			ucTmp |= (pSrc[2*i+1] - '0');
		}
		else if((pSrc[2*i+1]>='a')&&(pSrc[2*i+1]<='f'))
		{
			ucTmp |= (pSrc[2*i+1] - 'a' + 0x0A);
		}
		else if((pSrc[2*i+1]>='A')&&(pSrc[2*i+1]<='F'))
		{
			ucTmp |= (pSrc[2*i+1] - 'A' + 0x0A);
		}
		else
		{
			return -1;
		}
		
		pDst[j] = ucTmp;
    j++;
	}
	
	return 0;
}


int ChangeGB2312ToHex(char *pDst, char *pSrc)
{
	int i = 0, j = 0;
	
	while(1)
	{
		if(pSrc[j] == '%')
		{
			j++;
			ChangeHexStringToHexArray(&pSrc[j], 1, &pDst[i]);		
			j += 2;
			i++;
		}
		else
		{
			pDst[i] = pSrc[j] ;
			i++;
			j++;
		}
		
		if(pSrc[j] == '\0')
		{
			break;
		}
	}
	return i;
}


unsigned char CheckAndChangeUnicastIP(char * strIP, unsigned char ucLen, unsigned char *ucIP)
{
	unsigned char i=0,cnt = 0;
	unsigned int dwIPTemp = 0;
	//char * pTmp = NULL;
	
	for(i=0; i<ucLen; i++)
	{
		if((strIP[i] >= '0') && (strIP[i] <= '9'))
		{
			dwIPTemp = dwIPTemp*10;
			dwIPTemp += (strIP[i] - '0');
			
			if(dwIPTemp >= 256)
			{
				return 0;
			}
		}
		else
		{
			if(strIP[i] == '.')
			{
				ucIP[cnt] = (INT8U)dwIPTemp;
				dwIPTemp = 0;
				cnt++;
			}
			else
			{
				return 0;
			}
		}
	}
	
	if(cnt != 3)
	{
		return 0;
	}
	else
	{
		ucIP[cnt] = (INT8U)dwIPTemp;
		if((ucIP[0] == 0)&&(ucIP[1] == 0)&&(ucIP[2] == 0)&&(ucIP[3] == 0))
		{
			return 0;
		}
		if((ucIP[0] == 255)&&(ucIP[1] == 255)&&(ucIP[2] == 255)&&(ucIP[3] == 255))
		{
			return 0;
		}
		
		
		if(ucIP[0] > 223)
		{
			return 0;
		}
		
		
		return 1;
	}
}

unsigned char IPMaskCheck(unsigned char *pMask)
{
	unsigned int dwMask = 0;
	unsigned char i = 0;
	dwMask = pMask[0];
	dwMask <<= 8;
	dwMask |= pMask[1];
	dwMask <<= 8;
	dwMask |= pMask[2];
	dwMask <<= 8;
	dwMask |= pMask[3];
	

	while((dwMask&0x01) == 0)
	{
		dwMask = (dwMask>>1);
		i++;
	}

	// 全 0 或者全 1 都返回失败
	if((dwMask == 0) || (i == 0))
	{
		return 0;
	}


	while((dwMask&0x01) == 1)
	{
		dwMask = (dwMask>>1);
		
		i++;
	}

	if((dwMask == 0) && (i == 32))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



unsigned char CheckGateWayIP(unsigned char *pLocalIP, unsigned char *pMask, unsigned char *pLocalGW)
{
	unsigned char aLocalNetIP[4] = {0};
	unsigned char aLocalNetBoardCastIP[4] = {0};
	unsigned char aLocalNetIPTmp[4] = {0};
	unsigned char i = 0;	

	for(i=0;i<4;i++)
	{
		aLocalNetIP[i] = pLocalIP[i]&pMask[i];
		aLocalNetBoardCastIP[i] = aLocalNetIP[i]|(~pMask[i]);
		aLocalNetIPTmp[i] = pLocalGW[i]&pMask[i];
	}

	//printf("SubNetIP: %d.%d.%d.%d\r\n",aLocalNetIP[0],aLocalNetIP[1],aLocalNetIP[2],aLocalNetIP[3]);
	//printf("SubNetBCIP: %d.%d.%d.%d\r\n",aLocalNetBoardCastIP[0],aLocalNetBoardCastIP[1],aLocalNetBoardCastIP[2],aLocalNetBoardCastIP[3]);
	
	if(memcmp(aLocalNetIPTmp,aLocalNetIP,4) != 0)
	{
		return 2;  // 网关所属子网地址和本地IP所属子网地址不同
	}

	if(memcmp(pLocalGW,aLocalNetIP,4) == 0)
	{
		return 3;  // 网关等于子网地址
	}

	if(memcmp(pLocalGW,aLocalNetBoardCastIP,4) == 0)
	{
		return 4;  // 网关等于子网广播地址
	}

	if(memcmp(pLocalIP,aLocalNetIP,4) == 0)
	{
		return 5;  // IP等于子网地址
	}

	if(memcmp(pLocalIP,aLocalNetBoardCastIP,4) == 0)
	{
		return 6;  // IP等于子网广播地址
	}

	return 1;
	
}

unsigned char CheckIp(unsigned char ucIsMask,unsigned char * ucIP)
{
		if(ucIP[0] == 127)
		{
			return 0;
		}
		if(ucIP[0] == 0)//&&(ucIP[1] == 0)&&(ucIP[2] == 0)&&(ucIP[3] == 0))
		{
			return 0;
		}
		if((ucIP[0] == 255)&&(ucIP[1] == 255)&&(ucIP[2] == 255)&&(ucIP[3] == 255))
		{
			return 0;
		}
		if(1 == ucIsMask)
		{
			if(ucIP[3] == 255)
			{
				return 0;
			}
			
			return IPMaskCheck(ucIP);
			
		}
		else
		{
			if(ucIP[0] > 223)
			{
				return 0;
			}
		}
		
		return 1;
}

unsigned char CheckAndChangeIP(unsigned char ucIsMask,char * strIP, unsigned char ucLen, unsigned char *ucIP)
{
	unsigned char i=0,cnt = 0;
	unsigned int dwIPTemp = 0;
	//char * pTmp = NULL;
	
	for(i=0; i<ucLen; i++)
	{
		if((strIP[i] >= '0') && (strIP[i] <= '9'))
		{
			dwIPTemp = dwIPTemp*10;
			dwIPTemp += (strIP[i] - '0');
			
			if(dwIPTemp >= 256)
			{
				return 0;
			}
		}
		else
		{
			if(strIP[i] == '.')
			{
				ucIP[cnt] = (INT8U)dwIPTemp;
				dwIPTemp = 0;
				cnt++;
			}
			else
			{
				return 0;
			}
		}
	}
	
	if(cnt != 3)
	{
		return 0;
	}
	else
	{
		ucIP[cnt] = (INT8U)dwIPTemp;
		if(ucIP[0] == 127)
		{
			return 2;
		}
		if(ucIP[0] == 0)//&&(ucIP[1] == 0)&&(ucIP[2] == 0)&&(ucIP[3] == 0))
		{
			return 2;
		}
		if((ucIP[0] == 255)&&(ucIP[1] == 255)&&(ucIP[2] == 255)&&(ucIP[3] == 255))
		{
			return 2;
		}
		if(1 == ucIsMask)
		{
			if(ucIP[3] == 255)
			{
				return 0;
			}
			
			return IPMaskCheck(ucIP);
			
		}
		else
		{
			if(ucIP[0] > 223)
			{
				return 0;
			}
		}
		
		return 1;
	}
}

unsigned char CheckAndChangeMutiIP(char * strIP, unsigned char ucLen, unsigned char *ucIP)
{
	unsigned char i=0,cnt = 0;
	unsigned int dwIPTemp = 0;
	//char * pTmp = NULL;
	
	for(i=0; i<ucLen; i++)
	{
		if((strIP[i] >= '0') && (strIP[i] <= '9'))
		{
			dwIPTemp = dwIPTemp*10;
			dwIPTemp += (strIP[i] - '0');
			
			if(dwIPTemp >= 256)
			{
				return 0;
			}
		}
		else
		{
			if(strIP[i] == '.')
			{
				ucIP[cnt] = (INT8U)dwIPTemp;
				dwIPTemp = 0;
				cnt++;
			}
			else
			{
				return 0;
			}
		}
	}
	
	if(cnt != 3)
	{
		return 0;
	}
	else
	{
		ucIP[cnt] = (INT8U)dwIPTemp;
		if(ucIP[0] == 127)
		{
			return 2;
		}
		if(ucIP[0] == 0)//&&(ucIP[1] == 0)&&(ucIP[2] == 0)&&(ucIP[3] == 0))
		{
			return 2;
		}
		if((ucIP[0] == 255)&&(ucIP[1] == 255)&&(ucIP[2] == 255)&&(ucIP[3] == 255))
		{
			return 2;
		}
		
		if((ucIP[0] > 223)&&(ucIP[0] < 240))
		{
			return 1;
		}
		
		return 0;
	}
}





void urldecode(char *p)  
{  
	int i=0;  
	while(*(p+i))  
	{  
	   if ((*p=*(p+i)) == '%')  
	   {  
	    *p=*(p+i+1) >= 'A' ? ((*(p+i+1) & 0XDF) - 'A') + 10 : (*(p+i+1) - '0');  
	    *p=(*p) * 16;  
	    *p+=*(p+i+2) >= 'A' ? ((*(p+i+2) & 0XDF) - 'A') + 10 : (*(p+i+2) - '0');  
	    i+=2;  
	   }  
	   else if (*(p+i)=='+')  
	   {  
	    *p=' ';  
	   }  
	   p++;  
	}  
	*p='\0';  
}  



int CheckPassWordRule(char * pwd, unsigned char ucLen)
{
	unsigned char i = 0,ucSpecialCharFlg = 0,ucNumFlg = 0,ucCharFlg = 0;

	for(i=0;i<ucLen;i++)
	{
		if(((pwd[i]>=0x20)&&(pwd[i]<=0x2F))||((pwd[i]>=0x3A)&&(pwd[i]<=0x40))||((pwd[i]>=0x5B)&&(pwd[i]<=0x60))||((pwd[i]>=0x7B)&&(pwd[i]<=0x7E)))
		{
			ucSpecialCharFlg = 1;
		}
		else if((pwd[i]>=0x30)&&(pwd[i]<=0x39))
		{
			ucNumFlg = 1;
		}
		else if(((pwd[i]>=0x41)&&(pwd[i]<=0x5A))||((pwd[i]>=0x61)&&(pwd[i]<=0x7A)))
		{
			ucCharFlg = 1;
		}
		else
		{
			return -1;
		}
		
	}

	if((ucSpecialCharFlg == 0)||(ucNumFlg == 0)||(ucCharFlg == 0))
	{
		return -2;
	}

	return 0;
	
}



unsigned int StrToUnsignedint(char *data)
{
	unsigned int  tmp = 0;

	while(*data != '\0')
	{
		if((*data >= '0') && (*data <= '9'))
		{
			tmp *= 10;
			tmp += (*data -'0');
			data++;			
		}
		else
		{
			break;
		}
	}

	return tmp;
}


int  ChangeHexArrayToHexString(const unsigned char *pszData, int ilen, char * szData)
{
	int  usLoop = 0;
	char szTmp[8] = {0};
	int len = 0;
	
	for(usLoop = 0; usLoop < ilen; usLoop ++)
	{
		sprintf(szTmp, "%02X", (unsigned char)pszData[usLoop]);
		len += 2;
		strcat(szData, szTmp);
		memset(szTmp,0, sizeof(szTmp));
	}

	return len;
}


