#include <string>
#include "HardwareManager.h"
#include "ParseXmlApi.h"
const char xmlFileName[] = "CaseBinding.xml";

void analogTest(CHardwareManager * pHardwareManager)
{
	double wVol = 60;
	double rVol = 0;
	printf("Press any key to write....\n");
//	getchar();
	scanf("%lf", &wVol);
	//double wVol = 0.1;
	pHardwareManager->getCardManager("analog")->getCard(13)->writeData("397c2f6e-2fb0-47cc-adc8-a0832e10d533",(char*)&wVol,sizeof(wVol));// Resolver
	//getchar();
	int length = 8;
	struct timeval t;
	usleep(120000);

	//int  ret = pHardwareManager->getCardManager("analog")->getCard(7)->readData("73416fa3-8e72-4993-a815-1dbad7f1129c",(char*)&rVol,length,&t);
	//printf("Read data :   %lf %d-%d   ret = %d\n",rVol,t.tv_sec,t.tv_usec, ret);
}

void dioTest(CHardwareManager * pHardwareManager)
{
	printf("Press any key to write....\n");
	getchar();
	int wVol = 1;
	int rVol = 0;
	//pHardwareManager->getCardManager("discrete")->getCard(10)->writeData("6a6d5d90-b1ff-42e6-acea-20c9a5f98d33",(char*)&wVol,sizeof(wVol));
	while(1)
	{
		pHardwareManager->getCardManager("discrete")->getCard(10)->writeData("2e327803-6d23-4743-ad97-ab5a32689fed",(char*)&wVol,sizeof(wVol));
		usleep(200);

		//printf("Press any key to read....\n");
		getchar();
		int length = 4;
		struct timeval t;
		int ret = pHardwareManager->getCardManager("discrete")->getCard(10)->readData("d7e1594d-562f-4e14-b7ea-3f42a3110608",(char*)&rVol,length,&t);
		if(ret !=0)
		{
			printf("Read failed.(%d)\n",ret);
	//		return ;
		}
		printf("============================Read data : %d %d-%d\n",rVol,t.tv_sec,t.tv_usec);
	}
}
void a429Test(CHardwareManager * pHardwareManager)
{
	printf("Press any key to write a429....\n");
	getchar();
	string uuid = "e3482aa9-4744-4863-8af4-75448f73b1b4";
	char rVol;
	while(1)
	{
		//printf("1111111111111999999999999999999999999999999999999999999111111111111du wan le =========\n");
		//pHardwareManager->getCardManager("429")->getCard(16)->writeData("c995b37f-60a1-4cf9-86d7-5efa1c2f6606",(char*)&wVol,sizeof(wVol));
		int wVol = 0x01;
		pHardwareManager->getCardManager("429")->getCard(16)->writeData("00305763-3da5-421a-99b2-2986bf03913d",(char*)&wVol,sizeof(wVol));
		sleep(2);
		printf("Press any key to read a429....\n");
		getchar();
		int length = 4;
		struct timeval t;
		int ret = pHardwareManager->getCardManager("429")->getCard(16)->readData("565d903d-7a90-4964-aa65-929719326397",(char*)&rVol,length,&t);
		if(ret !=0)
		{
			printf("Read failed11.(%d)\n",ret);
	//		return ;
		}
		printf("Read data : %d %d-%d\n",rVol,t.tv_sec,t.tv_usec);
	}
}
void m1553Test(CHardwareManager * pHardwareManager)
{
	printf("Press any key to write 1553....\n");
	getchar();
	string uuid = "5fa0ad71-0a33-4de3-8404-6c615afaad18";
	//string uuid = "25b8263f-699a-434a-8c5e-978c8d40631d";
	const int DATA_LEN = 16;
	char wData[DATA_LEN]={0xA5,0xDA,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};
	//char yData[DATA_LEN]={0x0C,0x52};
	//char zData[DATA_LEN]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};
	char rData[DATA_LEN] = {0};
	pHardwareManager->getCardManager("1553")->getCard(7)->writeData(uuid,wData,DATA_LEN);

	printf("Press any key to read 1553....\n");
	getchar();
	int length = 16;
	struct timeval t;
	sleep(1);
	//uuid = "5fa0ad71-0a33-4de3-8404-6c615afaad18";
	//pHardwareManager->getCardManager("1553")->getCard(7)->writeData(uuid,zData,DATA_LEN);

	//sleep(1);
	int ret = pHardwareManager->getCardManager("1553")->getCard(7)->readData(uuid,rData,length,&t);
	if(ret !=0)
	{
		printf("Read failed.(%d)\n",ret);
		return ;
	}
	for(int i=0;i<length;i++)
	{
		printf("0x%02x ",rData[i]&0xFF);
	}
	printf("\n");
	printf("Time %d-%d\n",t.tv_sec,t.tv_usec);
	printf("\n");
}
void rs422Test(CHardwareManager * pHardwareManager)
{
	int count = 1 ;
	while(count--)
	{
		printf("Press any key to write rs422....\n");
		getchar();
		string uuid = "c47da126-bfce-40ee-b8a4-ef845288e3ac";
		char wData[]={0x01, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00};
		//char wData[]={0x01, 0xFA, 0xFF, 0xFA, 0x37, 0x0D, 0x0A};
		//char wData1[]={0x4F, 0x55, 0x54, 0x20, 0x4F, 0x4E, 0x0D, 0x0A};
		//char wData2[]={0x4f,0x55,0x54,0x20,0x4f,0x46,0x46,0x0d,0x0a};
		//char wData[]={0xff, 0xfa, 0xff, 0xfa, 0xd, 0x0, 0x11, 0x0, 0xc0, 0x0, 0x0, 0xee,0xeb};
		int length = sizeof(wData);
		pHardwareManager->getCardManager("422")->getCard(6)->writeData("81ee9fb4-6035-4f27-bacb-0f29f6efe7e6",wData,length);
		//sleep(2);
		//pHardwareManager->getCardManager("422")->getCard(6)->writeData("31a16de8-de60-41dd-a501-14f2899aef0b",wData1,length+1);

		printf("Press any key to read rs422....\n");
		getchar();
		char *rData = new char[length];
		struct timeval t;
		
		int ret = pHardwareManager->getCardManager("422")->getCard(6)->readData("40bf568b-bc0d-40fc-a9be-d6458c51bf7a",rData,length,&t);
		if(ret !=0)
		{
			printf("Read failed.(%d)\n",ret);
			return ;
		}
		for(int i=0;i<length;i++)
		{
			printf("0x%02x ",rData[i]&0xFF);
		}
		printf("\n");
		printf("Time %d-%d\n",t.tv_sec,t.tv_usec);
		
		printf("\n");
	}
}

int main()
{
	int ret = 0;
	//CHardwareManager * pHardwareManager = new CHardwareManager();
	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
	char configFileName[256] = "/home/V1.0.0/HiAutoTestServer/bin/Hardware.xml";
	printf("Load hardware resource ...\n");
	ret = pHardwareManager->loadResource("/home/V1.0.0/HiAutoTestServer/bin/Hardware.xml");///
	//ret = pHardwareManager->loadResource(configFileName);///
	if(ret != 0)
	{
        printf("error   Load hardware config ...\n");
	}
	else
	{
		printf("Load hardware config ...\n");
		//pHardwareManager->loadConfig("/home/V1.0.0/HardwareManager/ChannelConfig.xml");
		//printf("%s\n", GetXmlData("./ChannelConfig.xml"));
		//pHardwareManager->loadConfig(GetXmlData("/home/V1.0.0/HiAutoTestServer/bin/CardInfo.xml"));
		//pHardwareManager->loadConfig(GetXmlData("/home/V1.0.0/HardwareManager/CardInfo.xml"));
		printf("... ------------------success\n");
		//pHardwareManager->loadConfig(GetXmlData("/home/V1.0.0/HiAutoTestServer/bin/ChannelConfig.xml"));
		pHardwareManager->loadConfig(GetXmlData("/home/V1.0.0/HiAutoTestServer/bin/CardInfo.xml"));
		pHardwareManager->start();
		//a429Test(pHardwareManager);
		//analogTest(pHardwareManager);
		//dioTest(pHardwareManager);
		rs422Test(pHardwareManager);
		//m1553Test(pHardwareManager);
		printf("Press any key to stop....\n");
		getchar();

		pHardwareManager->stop();
		printf("Press any key to exit....\n");
		getchar();

	}
	return 0;



}
