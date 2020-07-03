---------------------------------------------------------------------
							Check GSM Network
---------------------------------------------------------------------
1.  AT
	Description: check module is connected
	Response: OK
2.  AT+CPIN? 
	Description: Check SIM Status
	Response: +CPIN:READY(Sim Card is found)
3. AT+CSQ
	Description: Check Signal Quality, Maximum signal quality is 31
	Response: +CSQ:30,0
4.  AT+COPS?
	Description: check sim card service provider
	Response: +COPS:0,0,"grameenPhone"
5.  AT+CREG?
	Description: Check SIM registered with the Network
	Response: +CREG: 0,1 -> indicated registered to home network;

http://www.electrodragon.com/w/Category:SIMCOM
---------------------------------------------------------------------
							Configure GSM
---------------------------------------------------------------------
1. AT+SAPBR=3,1,"Contype","GPRS"
	Description: Set the conneciton type GPRS
	Response: Ok
2. AT+SAPBR=3,1,"APN","www"
	Description: Set the APN
	Response: Ok
3. AT+SAPBR =1,1
	Description: Enable GPRS
	Response: Ok
4. AT+SAPBR=2,1
	Description: Check if the connection is setup properly. if we get back an IP address connection is ok
	Response: +SAPBR: 1,1,"100.120.204.132"
			OK

---------------------------------------------------------------------
							POST Request 
---------------------------------------------------------------------
1. AT+HTTPINIT
	Description: Enable HTTP Mode
	Response: Ok
2. AT+HTTPPARA="CID",1
	Description: Set HTTP Bearer Profile
	Response: Ok
3. AT+HTTPPARA="URL","<http post address>"
	Description: Set the URL of POST Request
	Response: Ok
4. AT+HTTPPARA="CONTENT","application/json"
	Description: Set the content type we want to send.it can also be "multipart/form-data etc.
	Response: OK
5. AT+HTTPDATA=192,10000
	Description: we tell the module that 192 bytes of data will be sent within 10 seconds.then modle will send Ok and DOWNLOAD command and wait 10 seconds to get data from controller. the data will be saved in local buffer of module
	Response: DOWNLOAD after body sent OK

6. AT+HTTPACTION=1
	Description: This will execute the HTTTP Post Command and buffer content will be postet to the server
	Response: Ok 
			  +HTTPACTION:1,200,141(in this URC 200 is response code, server receved 141 bytes of data)
4. AT+HTTPREAD
	Description: This command tell the module we want to read received data from server
	Response:   +HTTPREAD:141
				Successfully dumped 0 post variables.
				View it at http://www.posttestserver.com/data/2015/10/07/07.41.291690063480
				Post body was 0 chars long.
				OK

---------------------------------------------------------------------
							GET Request
---------------------------------------------------------------------


---------------------------------------------------------------------
		Bidirectional communication between Device and Server
---------------------------------------------------------------------
https://electronics.stackexchange.com/questions/166393/bidirectional-connection-between-a-device-and-a-webserver
https://vsblogs.wordpress.com/2013/11/28/tcp-connection-over-gprs-using-sim900-and-at-commands/

https://hackaday.com/2017/05/05/using-modern-c-techniques-with-arduino/