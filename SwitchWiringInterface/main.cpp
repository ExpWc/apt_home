#include "SwitchWiringInterface.h"
int main()
{
	//switchWiringByModel("192.168.1.121", "11011", "3a3d7148-fb35-4100-aac6-ff25de5ad7d9", "a90454d0-ad7e-11eb-ab9b-a374d85afd31");
	
	//switchWiringByIcd("192.168.1.121", "11011", "a1c7b09d-61af-4a77-b27e-b6d673c3f824", "a90454d0-ad7e-11eb-ab9b-a374d85afd31");
	
	//switchWiringByModel("192.168.1.121", "11011", "da760d49-24e9-4865-a06a-c2861e08c1f1", "3404c760-0366-11ee-a3da-b94b2a0f124d");
	
	//switchWiringByIcd("192.168.1.121", "11011", "59b0b4d7-1a53-4034-8c0b-5b3beefd828e", "3404c760-0366-11ee-a3da-b94b2a0f124d");
	
	//switchWiringByIcdJson("192.168.1.121", "11011", "[{\"status\":\"0\",\"type\":\
	\"nobus_analog\",\"uuid\":\"59b0b4d7-1a53-4034-8c0b-5b3beefd828e\"}]", "3404c760-0366-11ee-a3da-b94b2a0f124d");
	switchWiringByIcdJson("192.168.1.121", "11011", "[{\"status\":\"0\",\"type\":\
	\"nobus_analog\",\"uuid\":\"59b0b4d7-1a53-4034-8c0b-5b3beefd828e\"}]", "3404c760-0366-11ee-a3da-b94b2a0f124d");
	
	//string jsonString;
	//jsonString = "[{\"status\":\"1\",\"type\":\"nobus_analog\",\"uuid\":\"59b0b4d7-1a53-4034-8c0b-5b3beefd828e\"}]";
	//switchWiringByIcdJson("192.168.1.121", "11011", jsonString, "3404c760-0366-11ee-a3da-b94b2a0f124d");
	
	
	return 0;
}
