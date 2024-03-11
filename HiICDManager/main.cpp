#include <string>
#include "ICDManager.h"
#include "parseXmlApi.h"
#include "BaseField.h"
const char xmlFileName[] = "CaseBindingPre.xml";

//TiXmlDocument *pXmlManager;
int main()
{
    CICDManager *pICDManager = CICDManager::getInstance();//<<
    string str;
    str = string(GetXmlData(xmlFileName));
    pICDManager->init(str);
    string icdID;
    string fieldID1;
    long long tmp = 1;
#if 1
    icdID = "e10be1ae-fdff-47c0-bdf2-bebcb3014223";
	fieldID1 = "78e7e6ac-dc32-4b8a-bd1e-6de39149499d";

	if(pICDManager->getBlock(icdID) != NULL)
	{
		pICDManager->getBlock(icdID)->printFieldValue();
		printf("ICDManager getBlock >>>>>\n");
		if(pICDManager->getBlock(icdID)->getField(fieldID1) != NULL)
		{
			printf("ICDManager getField >>>>>\n");
			double doubleTmp = 130.5;
			memcpy(&tmp, &doubleTmp, 8);
			pICDManager->getBlock(icdID)->getField(fieldID1)->setDoubleData(tmp);

			double tmpDouble = 0;

			pICDManager->getBlock(icdID)->getField(fieldID1)->getData(&tmpDouble);
			printf("ICDManager unpackRxICD getData>>>>>%llf\n", tmpDouble);
		}
		pICDManager->getBlock(icdID)->packTxICD();

		//double tmpDouble = 0;
		printf("1\n");
		if(pICDManager->getBlock(icdID)->getField(fieldID1) != NULL)
		{
			printf("2\n");
			BaseField* pField = pICDManager->getBlock(icdID)->getField(fieldID1);
			printf("3 %d\n", pField);


			unsigned char rxData[MAX_PAYLOAD_LEN];
			int length = MAX_PAYLOAD_LEN;
			rxData[0] = 138;
			rxData[1] = 2;
			rxData[2] = 1;
			float a = 2.2;
			memcpy(&rxData[3], &a, 4);
			double b = 3.3;
			memcpy(&rxData[7], &b, 8);

			pICDManager->getBlock(icdID)->setRxData(rxData, length);//4<<
			pICDManager->getBlock(icdID)->unpackRxICD();//5<<
			printf("ICDManager unpackRxICD >>>>>\n");
			pICDManager->getBlock(icdID)->printFieldValue();
			double doubleTmp = 0;
			pField->getData(&doubleTmp);
			printf("ICDManager unpackRxICD >>>>>%lf\n", doubleTmp);
		}
	}
#endif

#if 0
    string icdID = "62507965-8f95-4a6f-b887-ce9309317210";
    string fieldID = "40191cf7-b37f-4c45-8ba1-94b7154e2acd";

    float floatTmp = 1.1;
    memcpy(&tmp, &floatTmp, 4);
    printf("ICDManager init >>>>>\n");
    if(pICDManager->getBlock(icdID) != NULL)
    {
        printf("ICDManager getBlock >>>>>\n");
        if(pICDManager->getBlock(icdID)->getField(fieldID) != NULL)
        {
            printf("ICDManager getField >>>>>\n");

            //float>>
            float floatTmp = 1.2;
            memcpy(&tmp, &floatTmp, 4);
            fieldID = "40191cf7-b37f-4c45-8ba1-94b7154e2acd";
            pICDManager->getBlock(icdID)->getField(fieldID)->setData(tmp);//1<<

            //double>>
            double doubleTmp = 4.5;
            memcpy(&tmp, &doubleTmp, 8);
            fieldID = "b92e9923-152e-4350-8fcf-2999daeae1b4";
            pICDManager->getBlock(icdID)->getField(fieldID)->setData(tmp);

            //bool>>
            string fieldID1 = "d9853ebb-18e4-4451-9754-e86eed2e5700";
            tmp = 1;
            pICDManager->getBlock(icdID)->getField(fieldID1)->setData(tmp);
            pICDManager->getBlock(icdID)->packTxICD();//2<<
            pICDManager->getBlock(icdID)->printFieldValue();

            //snd icd>>
            icdID = "903c4b4c-8a4b-4c40-96d6-7c2e08574ad4";
            fieldID1 = "5dceedb2-c75d-402a-9352-617c892a87da";
            tmp = 1;
            if(pICDManager->getBlock(icdID) != NULL)
            {
                printf("ICDManager getBlock >>>>>\n");
                if(pICDManager->getBlock(icdID)->getField(fieldID1) != NULL)
                {
                    printf("ICDManager getField >>>>>\n");
                    pICDManager->getBlock(icdID)->getField(fieldID1)->setData(tmp);
                }
            }

            //DIO ADDA>>
            icdID = "e10be1ae-fdff-47c0-bdf2-bebcb3014223";
            fieldID1 = "2c804436-b930-44b5-988a-8d06bef5d4e0";
            //doubleTmp = 2.5;
            //memcpy(&tmp, &doubleTmp, 8);
            tmp = 123;
            if(pICDManager->getBlock(icdID) != NULL)
            {
                printf("ICDManager getBlock DIO>>>>>\n");
                if(pICDManager->getBlock(icdID)->getField(fieldID1) != NULL)
                {
                    printf("ICDManager getField DIO>>>>>\n");
                    pICDManager->getBlock(icdID)->getField(fieldID1)->setData(tmp);
                    double tmpDouble = 0;
            
                    pICDManager->getBlock(icdID)->getField(fieldID1)->getData(&tmpDouble);
                    printf("ICDManager unpackRxICD >>>>>%lf\n", tmpDouble);
                }
                pICDManager->getBlock(icdID)->packTxICD();
                pICDManager->getBlock(icdID)->printFieldValue();
                double tmpDouble = 0;
                printf("1\n");
                if(pICDManager->getBlock(icdID)->getField(fieldID1) != NULL)
                {
                    printf("2\n");
                    BaseField* pField = pICDManager->getBlock(icdID)->getField(fieldID1);
                    printf("3 %d\n", pField);
                    pField->getData(&tmpDouble);
                    printf("ICDManager unpackRxICD >>>>>%lf\n", tmpDouble);
                }
            } 
            
            //TY_PACK_DATA txData;
            unsigned char txData[MAX_PAYLOAD_LEN];
            int length = MAX_PAYLOAD_LEN;
            int lengthRet = pICDManager->getBlock(icdID)->getTxData(txData, length);//3<<
            printf("ICDManager >>>>>lengthRet:%d txData:0x%x\n", lengthRet, txData);
            for(int i = 0; i < lengthRet; i++)
            {
                printf("%d ", txData[i]);
            }
            printf("\n");
            float txFloat = 0;
            memcpy(&txFloat, txData + 3, 4);
            printf("ICDManager txFloat %f >>>>>\n", txFloat);
            //TY_PACK_DATA rxData;
            unsigned char rxData[MAX_PAYLOAD_LEN];
            rxData[0] = 0;
            rxData[1] = 2;
            rxData[2] = 1;
            float a = 2.2;
            memcpy(&rxData[3], &a, 4);
            double b = 3.3;
            memcpy(&rxData[7], &b, 8);

            pICDManager->getBlock(icdID)->setRxData(rxData, length);//4<<
            pICDManager->getBlock(icdID)->unpackRxICD();//5<<
            printf("ICDManager unpackRxICD >>>>>\n");
            pICDManager->getBlock(icdID)->printFieldValue();

            double tmpDouble = 0;
            
            pICDManager->getBlock(icdID)->getField(fieldID)->getData(&tmpDouble);//6<<
            printf("ICDManager unpackRxICD >>>>>%lf\n", tmpDouble);


        }
    }
#endif
    printf("ICDManager finish >>>>>\n");
    return 0;
}
