#include "WeEncrypt.h"
#include "blowfish.h"

namespace We
{
	//------------------------------------------------------------------------
	Encrypt::Encrypt()
	{
		m_Encrypt = new CBlowFish();
	}
	//------------------------------------------------------------------------
	Encrypt::~Encrypt()
	{
		if( m_Encrypt != 0 )
		{
			delete (CBlowFish*)m_Encrypt;
			m_Encrypt = 0;
		}
	}
	//------------------------------------------------------------------------
	void Encrypt::SetKey(unsigned char key[], int keybytes )
	{
		((CBlowFish*)m_Encrypt)->Initialize( key, keybytes );
	}
	//------------------------------------------------------------------------
	void Encrypt::SetDefaultKey()
	{
		unsigned char key[] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14 };
		SetKey( key, sizeof(key)/sizeof(key[0]) );
	}
	//------------------------------------------------------------------------
	int Encrypt::GetOutputSize(int inputSize )
	{
		CBlowFish* blowFish = (CBlowFish*)m_Encrypt;
		return blowFish->GetOutputLength( inputSize );
	}
	//------------------------------------------------------------------------
	void Encrypt::Encode( unsigned char* inputData, int inputDataSize, unsigned char* outputData )
	{
		// inputDataSize必须是8的倍数
		assert( (inputDataSize%8) == 0 );
		CBlowFish* blowFish = (CBlowFish*)m_Encrypt;
		blowFish->Encode( inputData, outputData, inputDataSize );
	}
	//------------------------------------------------------------------------
	void Encrypt::Decode( unsigned char* inputData, int inputDataSize, unsigned char* outputData )
	{
		// inputDataSize必须是8的倍数
		assert( (inputDataSize%8) == 0 );
		CBlowFish* blowFish = (CBlowFish*)m_Encrypt;
		blowFish->Decode( inputData, outputData, inputDataSize );
	}
	//------------------------------------------------------------------------
}