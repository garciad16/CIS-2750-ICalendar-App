/**Daniel Garcia
 * Student Number: 1022106
 * CIS 2750 Assignment 2: W19
 **/

#include "CalendarParser.h"
#include "LinkedListAPI.h"
#include "Helpers.h"

char* eventOptionalListToJSON(const List* eventList){
	char *stringReturn;
	List* temp = (List*)eventList;
	char* temp2 = (char*)eventList;
	Event *event = (Event*)malloc(sizeof(Event));
	ListIterator iter;
	void *data;
	char begin[256] = "[";
	char mid[256] = ",";
	char end[256] = "]";
	int length = 0;
	char nullStr[256] = "[]";
	
	stringReturn = (char*)malloc(sizeof(char) + 200);
	
	if(eventList == NULL || strcmp(temp2, "") == 0){
		strcpy(stringReturn, nullStr);
		return stringReturn;
	}
	
	strcpy(stringReturn, begin);
	
	iter = createIterator(temp);
	while(iter.current != NULL){
		data = nextElement(&iter);
		event = (Event*)data;
		char *evtJSFunc = eventOptionalToJSON(event);
		//printf("Evt JS Func Test: %s\n", evtJSFunc);
		stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + 200));
		strcat(stringReturn, evtJSFunc);
		if(iter.current == NULL){
			strcat(stringReturn, end);
		}
		else {
			strcat(stringReturn, mid);
		}
		free(evtJSFunc);
		length = strlen(stringReturn);
		stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + 200));
	}
	
	return stringReturn;
}

char *calOpenOptionalEvent(char *filename){
	Calendar *cal;
	ICalErrorCode err;
	char *stringReturn;
	stringReturn = (char*)malloc(sizeof(char) + 200);

	printf("C testing calOPEN function in C\n");
	err = createCalendar(filename, &cal);
	if(err != OK){
		return NULL;
	}
	stringReturn = eventOptionalListToJSON(cal->events);

	printf("String Optional returns: %s\n", stringReturn);
	return stringReturn;
}


char* eventOptionalToJSON(const Event* event){
	int length = 0;
	char *stringReturn;
	char stringTemp2[64] = "";
	char stringTemp3[64] = "";
	char *tempEvent = (char*)event;
	void *data;
	Property *prop;
	char semi [50] = "{\"organizer\":\"";
	char semi2 [50] = "\",\"location\":\"";
	char end[50] = "\"}";
	char nullEvent[50] = "{}";
	int flag2 = 0;
	int flag3 = 0;
	ListIterator iter = createIterator(event->properties);
	ListIterator subIter = createIterator(event->alarms);
	
	if(tempEvent == NULL){
		stringReturn = (char*)malloc(sizeof(char));
		strcpy(stringReturn, nullEvent);
		return stringReturn;
	}
	
	int numProps = 0;
	int numAlarms = 0;
	
	if(event->properties != NULL){
		while(iter.current != NULL){
			data = nextElement(&iter);
			prop = (Property*)data;
			if(strcmp(prop->propName, "ORGANIZER") == 0){
				flag2 = 1;
				strcpy(stringTemp2, prop->propDescr);
				numProps++;
			}
			if(strcmp(prop->propName, "LOCATION") == 0){
				flag3 = 1;
				strcpy(stringTemp3, prop->propDescr);
				numProps++;
			}
			else{
				
				numProps++;
			}
		}
		numProps = numProps + 3;
	}
	
	if(event->alarms != NULL){
		while(subIter.current != NULL){
			numAlarms++;
			nextElement(&subIter);
		}
	}
	
	stringReturn = (char*)malloc(sizeof(char) + 30000);
	
	strcpy(stringReturn, semi);
	if(flag2 == 1){
		strcat(stringReturn, stringTemp2);
	}
	else{
		strcat(stringReturn, "");
	}
	
	length = strlen(stringReturn);
    stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + 200));
	
	strcat(stringReturn, semi2);
	if(flag3 == 1){
		strcat(stringReturn, stringTemp3);
	}
	else{
		strcat(stringReturn, "");
	}
	
	length = strlen(stringReturn);
    stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + 200));
	
	strcat(stringReturn, end);
	
	flag2 = 0;
	flag3 = 0;
	
	printf("eventToJSON Optional Test: %s\n", stringReturn);
	
	return stringReturn;
}

Event* JSONtoDT(const char* str, Event *tempEvent){
	//Event* tempEvent = NULL;
	//tempEvent = (Event*)malloc(sizeof(Event));
	char line[256];
	char *subString;
	
	tempEvent->creationDateTime.UTC = 0;
	
	if(str == NULL){
		return NULL;
	}
	
	if(!str){
		return NULL;
	}
	
	strcpy(line, str);
	
	subString = strtok(line, "{\"startDate\":\"");
	strcpy(tempEvent->startDateTime.date, subString);
	
	subString = strtok(NULL, "\"");
	subString = strtok(NULL, "\":\"");
	
	subString = strtok(NULL, "\":\"");
	strcpy(tempEvent->startDateTime.time, subString);
	
	subString = strtok(NULL, "\":\"");	
	subString = strtok(NULL, "\":\"");
	
	subString = strtok(NULL, "\":\"");
	strcpy(tempEvent->creationDateTime.date, subString);
	
	subString = strtok(NULL, "\":\"");
	subString = strtok(NULL, "\":\"");
	
	subString = strtok(NULL, "\":\"");
	strcpy(tempEvent->creationDateTime.time, subString);	

	return tempEvent;
}

char *newEvent(char *filename, char* uidString, char* dtString){
	Calendar *cal;
	Event *event;
	ICalErrorCode err;
	char *stringReturn = "OK";
	char *stringErr = "NULL";
	
	if(strcmp(dtString, "{}") == 0){
		printf("INVALID DT dates\n");
		return stringErr;
	}

	err = createCalendar(filename, &cal);
	if(err != OK){
		return stringErr;
	}
	else if (err == OK){
		err = validateCalendar(cal);
		if (err == OK){
			if(strcmp(uidString, "{}") != 0 && strcmp(dtString, "{}") != 0){
				printf("Adding new event...\n");
				event = JSONtoEvent(uidString);
				event = JSONtoDT(dtString, event);
				addEvent(cal, event);
				writeCalendar(filename, cal);
			}
		}
		else {
			printf("INVALID CALENDAR\n");
			return stringErr;
		}
	}	
	//printf("stringReturn is: %s\n", stringReturn);
	deleteCalendar(cal);
	return stringReturn;
}

char *newCal(char *filename, char* calString, char* uidString, char* dtString){
	Calendar *cal;
	Event *temp;
	ICalErrorCode err;
	char *stringReturn = "OK";
	char *stringErr = "NULL";
	
	if(strcmp(dtString, "{}") == 0){
		printf("INVALID DT dates\n");
		return stringErr;
	}
	
	cal = JSONtoCalendar(calString);
	temp = (Event*)cal;
	
	if(strcmp(uidString, "{}") != 0 && strcmp(dtString, "{}") != 0){
		temp = JSONtoEvent(uidString);
		temp = JSONtoDT(dtString, temp);
		addEvent(cal, temp);
	}
	
	//cal = (Calendar*)temp;
	
	//NOTE to self: calling writeCalendar causes it to create an ics file in uploads, may cause error in app.js
	
	err = validateCalendar(cal);
	if (err == OK){
		writeCalendar(filename, cal);
		printf("Write Calendar was a success\n");
	}
	else {
		printf("Write Calendar was a failure\n");
		return stringErr;
	}
	
	deleteCalendar(cal);
	return stringReturn;
}

//returns the calendar obj of createCalendar()
char *calOpen(char *filename){
	Calendar *cal;
	ICalErrorCode err;
	char *stringReturn;
	stringReturn = (char*)malloc(sizeof(char) + 200);

	printf("C testing calOPEN function in C\n");
	err = createCalendar(filename, &cal);
	if(err != OK){
		return NULL;
	}
	stringReturn = calendarToJSON(cal);

	//printf("calOpen returns: %s\n", stringReturn);
	return stringReturn;
}

char *calOpenEvent(char *filename){
	Calendar *cal;
	ICalErrorCode err;
	char *stringReturn;
	stringReturn = (char*)malloc(sizeof(char) + 200);

	printf("C testing calOPEN function in C\n");
	err = createCalendar(filename, &cal);
	if(err != OK){
		return NULL;
	}
	stringReturn = eventListToJSON(cal->events);

	printf("calOpen returns: %s\n", stringReturn);
	return stringReturn;
}

ICalErrorCode createCalendar(char* fileName, Calendar** obj) {
	char folded[2048];
	char fileLine[2048];
	char temp[2048];
	char key[2048];
	char value[2048];
	char *tok;
	char *tempFile;
	char unFoldedChr = '0';
	int calEnd = 0;
	int inAlarm = 0;
	int inEvent = 0;
	int evtAlarm = 0;
	int calEvt = 0;
	int eventProp = 0;
	int alarmProp = 0;
	int calProp = 0;
	Alarm *alarm;
	Calendar *cal = (Calendar*)malloc(sizeof(Calendar));
	Event *event;
	FILE *file;
	Property *prop;
	
	cal->version = 0.0;
	strcpy(cal->prodID, "");
	cal->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
	cal->properties->head = NULL;
	cal->properties->tail = NULL;
	cal->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
	cal->events->head = NULL;
	cal->events->tail = NULL;		

	//Error if obj is not of type Calendar
	if (obj == NULL){
		return errorCode(cal, NULL, obj, INV_FILE);
	}
	//Error if file is NULL or empty
	else if(!fileName || strcmp(fileName, "") == 0){
		return errorCode(cal, NULL, obj, INV_FILE);
	}
	printf("Is this obj NULL?\n");

	tempFile = (char*)malloc(sizeof(char) * (strlen(fileName) + 1));
  	strcpy(tempFile, fileName);

	//tokenizes with '.'
	tok = strtok(tempFile, ".");
  	if(tok != NULL) {
    	tok = strtok(NULL, ".");
   	}
	printf("DOES IT TOKENIZE?\n");
	//if file doesn't contain ics, return INV_FILE error
 	if(!tok || strcmp(tok, "ics") != 0) {
    	free(tempFile);
		return errorCode(cal, NULL, obj, INV_FILE);
	}
	printf("IS IT AN ICS FILE\n");
	//free tempfile
	free(tempFile);
  	file = fopen(fileName, "r");

	//If file doesn't exist, return INV_FILE
	if (!file){
		return errorCode(cal, NULL, obj, INV_FILE);
	}
	printf("DOES THIS FILE EXIST?\n");
	//If it doesn't read the line, return INV_FILE
	if (!fgets(fileLine, sizeof(fileLine), file)){
		return errorCode(cal, file, obj, INV_FILE);
	}
	//printf("Is this file NULL?\n");  
	unfoldLine(fileLine);
	while(isspace(unFoldedChr = fgetc(file))) {
		fgets(folded, sizeof(folded), file);
		unfoldLine(folded);
		strcat(fileLine, folded);
	}
  
	if(strstr(fileLine, "BEGIN:VCALENDAR") == NULL) {
		return errorCode(cal, file, obj, INV_CAL);
	}

	while(fgets(fileLine, sizeof(fileLine), file)) {
		if((fileLine[strlen(fileLine) - 2] != '\r' && fileLine[strlen(fileLine) - 2] != '\n')) {
			return errorCode(cal, file, obj, INV_FILE);
		} 

		if(unFoldedChr != '0') {
			strcpy(temp, fileLine);
			strcpy(fileLine, "");
      
			fileLine[0] = unFoldedChr;
			fileLine[1] = '\0';
			strcat(fileLine, temp);
      
			unFoldedChr = '0';
		}
    
		if(fileLine[0] == ';') {
			continue;
		}
    
		unfoldLine(fileLine);
		while(isspace(unFoldedChr = fgetc(file))) {
			fgets(folded, sizeof(folded), file);
			unfoldLine(folded);
			strcat(fileLine, folded);
		}
		
		printf("%s\n", fileLine);

		tok = strtok(fileLine, ":;");
		strcpy(key, tok);
    
		tok = strtok(NULL, "");  
		
		if(!tok && inAlarm) {
			return errorCode(cal, file, obj, INV_ALARM);
		} 
		else if(!tok && inEvent) {
			if(strcmp(key, "DTSTAMP") == 0) {
				return errorCode(cal, file, obj, INV_DT);
			} 
			else {
				return errorCode(cal, file, obj, INV_EVENT);
			}
		} 
		else if(!tok) {
			if(strstr(key, "VERSION") != NULL) {
				return errorCode(cal, file, obj, INV_VER);
			} 
			else if(strstr(key, "PRODID") != NULL) {
				return errorCode(cal, file, obj, INV_PRODID);
			} 
		}
		strcpy(value, tok);

		if(strstr(key, "BEGIN") != NULL) {
			if(strstr(value, "VEVENT") != NULL) {     
				inEvent = 1;           
				event = (Event*)malloc(sizeof(Event));
				event->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms); 
				event->alarms->head = NULL;
				event->alarms->tail = NULL;
				strcpy(event->UID, "");
				strcpy(event->creationDateTime.date, "");
				strcpy(event->startDateTime.date, "");
				event->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
				event->properties->head = NULL;
				event->properties->tail = NULL;
			} 
			else if(strstr(value, "VALARM") != NULL) {
				inAlarm = 1;   
				alarm = (Alarm*)malloc(sizeof(Alarm));
				alarm->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
				alarm->properties->head = NULL;
				alarm->properties->tail = NULL;
				strcpy(alarm->action, "");
				alarm->trigger = NULL;
			} 
			
		} 
		else if(strcmp(key, "END") == 0) {
			if(strstr(value, "VCALENDAR") != NULL) {
				calEnd = 1;
			} 
			else if(strstr(value, "VEVENT") != NULL) {
				
				if(strcmp(event->UID, "") == 0 || strcmp(event->creationDateTime.date, "") == 0) {
					return errorCode(cal, file, obj, INV_EVENT);
				}
	
				calEvt++;       
				inEvent = 0;
				insertBack(cal->events, event);
			} 
			else if(strstr(value, "VALARM") != NULL) {
				if(strcmp(alarm->action, "") == 0 || !alarm->trigger) {
					return errorCode(cal, file, obj, INV_ALARM);
				}
        
				evtAlarm++;
				inAlarm = 0;
				insertBack(event->alarms, alarm);
			} 
		} 
		else if(strstr(key, "PRODID") != NULL) {
			if(strcmp(cal->prodID, "") != 0) {
				return errorCode(cal, file, obj, DUP_PRODID);
			}
		
			strcpy(cal->prodID, value);
		} 
		else if(strstr(key, "VERSION") != NULL) {
			if(cal->version != 0.0) {
				return errorCode(cal, file, obj, DUP_VER);
			} 
      
			cal->version = atof(value);
			if(cal->version == 0.0) {
				return errorCode(cal, file, obj, INV_VER);
			}
		} 
		else if(calEnd == 0 && inEvent == 0 && inAlarm == 0) {
			calProp++;	
			prop = (Property*)malloc(sizeof(Property) + (sizeof(char) * strlen(tok) + 1));
			strcpy(prop->propName, key);
			strcpy(prop->propDescr, value);
			insertBack(cal->properties, prop);
		} 
		else {
			if(strstr(key, "DTSTAMP") != NULL) {
				if(strcmp(event->creationDateTime.date, "") != 0) {
					return errorCode(cal, file, obj, INV_EVENT);
				}
        
				tok = strtok(value, "T");
				if(!tok || strlen(tok) != 8) {
					return errorCode(cal, file, obj, INV_DT);
				}

				strcpy(event->creationDateTime.date, tok);
				tok = strtok(NULL, "T");
				if(!tok || (strlen(tok) != 7 && strlen(tok) != 6)) {
					return errorCode(cal, file, obj, INV_DT);
				}

				event->creationDateTime.UTC = 0;
				if(tok[6] == 'Z') {
					event->creationDateTime.UTC = 1;
				}

				tok[6] = '\0';
				strcpy(event->creationDateTime.time, tok);
			} 
			else if(strstr(key, "DTSTART") != NULL) {
				if(strcmp(event->startDateTime.date, "") != 0) {
					return errorCode(cal, file, obj, INV_EVENT);
				}
        
				tok = strtok(value, "T");
				if(!tok || strlen(tok) != 8) {
					return errorCode(cal, file, obj, INV_DT);
				}

				strcpy(event->startDateTime.date, tok);
				tok = strtok(NULL, "T");
				if(!tok || (strlen(tok) != 7 && strlen(tok) != 6)) {
					return errorCode(cal, file, obj, INV_DT);
				}

				event->startDateTime.UTC = 0;
				if(tok[6] == 'Z') {
					event->startDateTime.UTC = 1;
				}

				tok[6] = '\0';
				strcpy(event->startDateTime.time, tok);
				
				/*char *toPrint = dtToJSON(event->startDateTime);
				printf("%s\n", toPrint);
                free(toPrint);*/
			}
			else if(strstr(key, "UID") != NULL) {
				if(strcmp(event->UID, "") != 0) {
					return errorCode(cal, file, obj, INV_EVENT);
				}
        
				strcpy(event->UID, value);
			} 
			else if(inEvent == 1 && inAlarm == 0){
				eventProp++;
				prop = (Property*)malloc(sizeof(Property) + (sizeof(char) * strlen(value) + 100));
				strcpy(prop->propName, key);
				strcpy(prop->propDescr, value);
				insertBack(event->properties, prop);
			}  
			else {
				if(strstr(key, "TRIGGER") != NULL) {
					if(alarm->trigger) {
						return errorCode(cal, file, obj, INV_ALARM);
					}
          
					alarm->trigger = (char*)malloc(sizeof(char) * strlen(tok) + 1);
					strcpy(alarm->trigger, tok);
				} 
				else if(strstr(key, "ACTION") != NULL) {
					if(strcmp(alarm->action, "") != 0) {
						return errorCode(cal, file, obj, INV_ALARM);
					}    
					strcpy(alarm->action, value);
				} 
				else if(inEvent == 1 && inAlarm == 1) {	
          				
					alarmProp++;
					prop = (Property*)malloc(sizeof(Property) + (sizeof(char) * strlen(value) + 1));
					strcpy(prop->propName, key);
					strcpy(prop->propDescr, value);
					insertBack(alarm->properties, prop);
				} 
			}
		}
	}
	
	if(cal->version == 0.0 || strcmp(cal->prodID, "") == 0 || !cal->events || !calEnd) {
		return errorCode(cal, file, obj, INV_CAL);
	} 
	

	*obj = cal;
	fclose(file);
	return OK;
}

ICalErrorCode writeCalendar(char* fileName, const Calendar* obj){
		FILE *file;
		Alarm *alarm;
		Event *event;
		ListIterator iterCalProp;
		ListIterator iterCalEvt;
		ListIterator iterEvtProp;
		ListIterator iterEvtAlarm;
		ListIterator iterAlarmProp;
		Property *prop;		
			
		if (!obj){
				return WRITE_ERROR;
		}
		
		file = fopen(fileName, "w");
		if (!file){
			return WRITE_ERROR;	
		}
		
		fprintf(file, "BEGIN:VCALENDAR\r\n");
		fprintf(file, "VERSION:%f\r\n", obj->version);
		fprintf(file, "PRODID:%s\r\n", obj->prodID);
		if(obj->properties != NULL){
			iterCalProp = createIterator(obj->properties);
			printf("Writing cal->properties...\n");
			while(iterCalProp.current != NULL){
				prop = (Property*)nextElement(&iterCalProp);
				fprintf(file, "%s:%s\r\n", prop->propName, prop->propDescr);
			}
		}
		
		iterCalEvt = createIterator(obj->events);
		printf("Writing cal->events...\n");
		while(iterCalEvt.current != NULL){
				event = (Event*)nextElement(&iterCalEvt);
				fprintf(file, "BEGIN:VEVENT\r\n");
				fprintf(file, "UID:%s\r\n", event->UID);
				if (event->creationDateTime.UTC){
					fprintf(file, "DTSTAMP:%sT%sZ\r\n", event->creationDateTime.date, event->creationDateTime.time);		
				}
				else{
					fprintf(file, "DTSTAMP:%sT%s\r\n", event->creationDateTime.date, event->creationDateTime.time);		
				}	
				if (event->startDateTime.UTC){
						fprintf(file, "DTSTART:%sT%sZ\r\n", event->startDateTime.date, event->startDateTime.time);
				}
				else{
					fprintf(file, "DTSTART:%sT%s\r\n", event->startDateTime.date, event->startDateTime.time);
				}
				if(event->properties != NULL){
					iterEvtProp=createIterator(event->properties);
					printf("Writing event->properties...\n");
					while(iterEvtProp.current != NULL){
							prop = (Property*)nextElement(&iterEvtProp);
							if (strcmp(prop->propName, "ORGANIZER") == 0){
								fprintf(file, "%s;%s\r\n", prop->propName, prop->propDescr);
							}
							else {
								fprintf(file, "%s:%s\r\n", prop->propName, prop->propDescr);
							}
					}
				}
				if (event->alarms != NULL){
					iterEvtAlarm = createIterator(event->alarms);
					printf("Writing event->alarms...\n");
					while(iterEvtAlarm.current != NULL){
						alarm = (Alarm*)nextElement(&iterEvtAlarm);
						fprintf(file, "BEGIN:VALARM\r\n");
						fprintf(file, "TRIGGER;%s\r\n", alarm->trigger);
						fprintf(file, "ACTION:%s\r\n", alarm->action);
					
						if(alarm->properties != NULL){
							iterAlarmProp = createIterator(alarm->properties);
							printf("Writing alarm->properties...\n");
							while(iterAlarmProp.current != NULL){
								prop = (Property*)nextElement(&iterAlarmProp);
								if (strcmp(prop->propName, "ATTACH") == 0){
									fprintf(file, "%s;%s\r\n", prop->propName, prop->propDescr);
								}
								else {
									fprintf(file, "%s:%s\r\n", prop->propName, prop->propDescr);
								}
							}
							
						}					
						fprintf(file, "END:VALARM\r\n");
					}
				}
			fprintf(file, "END:VEVENT\r\n");
		}		
		fprintf(file, "END:VCALENDAR\r\n");
		
		fclose(file);
		return OK;
}

ICalErrorCode validateCalendar(const Calendar* obj){
	void *data;
	Alarm *alarm;
	Event *event;
	ListIterator iterCalProp;
	ListIterator iterCalEvt;
	ListIterator iterEvtProp;
	ListIterator iterEvtAlarm;
	ListIterator iterAlarmProp;
	Property *prop;
	
	//missing required calendar components, or empty values
	if (!obj || obj->version == 0.0 || strcmp(obj->prodID, "") == 0){
		return INV_CAL;
	}
	
	if(obj->properties == NULL){
		return INV_CAL;
	}
	
	if(obj->properties != NULL){
		iterCalProp = createIterator(obj->properties);
		printf("Validating cal->properties...\n");
		while(iterCalProp.current != NULL){
			data = nextElement(&iterCalProp);
			prop = (Property*)data;
			
			//Compare properties, if not matched then invalid property, checks for duplicates
			if((strstr(prop->propName, "CALSCALE") == NULL && strstr(prop->propName, "METHOD") == NULL)){
				return INV_CAL;
			}
			if (findElement(obj->properties, diffProperty, data)){
				return INV_CAL;
			}
			if(strcmp(prop->propName, "CALSCALE") == 0){
				if(strcmp(prop->propDescr, "") == 0){
					return INV_CAL;
				}
			}
		}
	}
	//missing events list
	if(obj->events == NULL){
		return INV_CAL;	
	}
	
	iterCalEvt = createIterator(obj->events);
	printf("Validating cal->events...\n");
	while(iterCalEvt.current != NULL){
		event = (Event*)nextElement(&iterCalEvt);
		//empty required properties
		if (strlen(event->startDateTime.date) != 8 || strlen(event->startDateTime.time) != 6){
			return INV_EVENT;
		}
		if(strcmp(event->UID, "") == 0 || strlen(event->creationDateTime.date) != 8 || strlen(event->creationDateTime.time) != 6){
			return INV_EVENT;
		}
		//If the list is Empty
		if(event->properties == NULL){
			return INV_EVENT;
		}
		if(event->properties != NULL ){
			iterEvtProp = createIterator(event->properties);
			printf("Validating event->properties...\n");
			while(iterEvtProp.current != NULL){
				data = nextElement(&iterEvtProp);
				prop = (Property*)data;
				
				if(strstr(prop->propName, "DTEND") != NULL){
					//if DURATION is found, error
					if(findElement(event->properties, compareProperty, "DURATION")){
						return INV_EVENT;
					}
				}
				else if(strstr(prop->propName, "DURATION") != NULL){
					//if DTEND is found, error
					if(findElement(event->properties, compareProperty, "DTEND")){
						return INV_EVENT;
					}
				}
				else if(strstr(prop->propName, "CLASS") != NULL || strstr(prop->propName, "CREATED") != NULL || strstr(prop->propName, "DESCRIPTION") != NULL
				|| strstr(prop->propName, "GEO") != NULL || strstr(prop->propName, "LAST-MODIFIED") != NULL || strstr(prop->propName, "LOCATION") != NULL
				|| strstr(prop->propName, "ORGANIZER") != NULL || strstr(prop->propName, "PRIORITY") != NULL || strstr(prop->propName, "SEQUENCE") != NULL
				|| strstr(prop->propName, "STATUS") != NULL || strstr(prop->propName, "SUMMARY") != NULL || strstr(prop->propName, "TRANSP") != NULL
				|| strstr(prop->propName, "URL") != NULL || strstr(prop->propName, "RECURRENCE-ID") != NULL || strstr(prop->propName, "RRULE") != NULL){
					//Compare properties, if not matched then invalid property, checks for duplicates
					if(findElement(event->properties, diffProperty, data)){
						return INV_EVENT;
					}
				}
				//if Property is none of these, return INV_EVENT
				else if((strstr(prop->propName, "ATTACH") == NULL) && strstr(prop->propName, "ATTENDEE") == NULL && strstr(prop->propName, "COMMENT") == NULL 
				&& strstr(prop->propName, "CATEGORIES") == NULL && strstr(prop->propName, "CONTACT") == NULL && strstr(prop->propName, "EXDATE") == NULL 
				&& strstr(prop->propName, "REQUEST-STATUS") == NULL && strstr(prop->propName, "RELATED-TO") == NULL && strstr(prop->propName, "RESOURCES") == NULL 
				&& strstr(prop->propName, "RDATE") == NULL){
						return INV_EVENT;
				}
			}		
		}
		if(event->alarms == NULL){
			return INV_EVENT;
		}
		if(event->alarms != NULL){
			iterEvtAlarm = createIterator(event->alarms);
			printf("Validating event->alarms...\n");
			while(iterEvtAlarm.current != NULL){
				alarm = (Alarm*)nextElement(&iterEvtAlarm);
				//if alarms have empty value, error
				if(strcmp(alarm->action, "") == 0 || !alarm->trigger){
					return INV_ALARM;
				}
				if(alarm->properties == NULL){
						return INV_ALARM;
				}
				if(alarm->properties != NULL){
					iterAlarmProp = createIterator(alarm->properties);
					printf("Validating alarm->properties...\n");
					while(iterAlarmProp.current != NULL){
						data = nextElement(&iterAlarmProp);
						prop = (Property*)data;
		
						if(strstr(prop->propName, "DURATION") != NULL){
							//if REPEAT is not found with DURATION, error
							if(!findElement(alarm->properties, compareProperty, "REPEAT")){
								return INV_ALARM;
							}
							if (findElement(alarm->properties, diffProperty, data)){
								return INV_ALARM;
							}
						}
						else if(strstr(prop->propName, "REPEAT") != NULL){
							//if DURATION is not found with REPEAT, error
							if(!findElement(alarm->properties, compareProperty, "DURATION")){
								return INV_ALARM;
							}
							if (findElement(alarm->properties, diffProperty, data)){
								return INV_ALARM;
							}
						}
						else if(strstr(prop->propName, "ATTACH") != NULL){
							//Compare properties, if not matched then invalid property
							if(findElement(alarm->properties, diffProperty, data)){
								return INV_ALARM;
							}
						}
						else {
							//Else if not any of the other properties then property doesn't exist, error
							return INV_ALARM;
						}
					}
				}
			}
		}	
	}
	return OK;
}

void deleteCalendar(Calendar* obj) {
	
	if (obj->events != NULL){
		freeList(obj->events);	
	}
	
	if(obj->properties != NULL){
		freeList(obj->properties);
	}
	
	free(obj);
}

char* printCalendar(const Calendar* obj) {
	char *toReturn;
	char *temp;
	char *temp2;
	int len;
	
	if(!obj) {
		return NULL;
	}
  
	toReturn = (char*)malloc(sizeof(char) * (strlen(obj->prodID) + 100));
	sprintf(toReturn, "\nCalendar:\n version = %f\n prodID = %s\n Calendar Properties:\n", obj->version, obj->prodID);
	len = strlen(toReturn);
	
	if(obj->properties != NULL){
		temp2 = toString(obj->properties);
		if (temp2){
			toReturn = (char*)realloc(toReturn, sizeof(char) * (len + strlen(temp2) + 100));
			strcat(toReturn, temp2);
			free(temp2);
		}
	}
	if(obj->events != NULL){
		temp = toString(obj->events);
		if(temp) {
			toReturn = (char*)realloc(toReturn, sizeof(char) * (len + strlen(temp) + 100));
			strcat(toReturn, temp);
			free(temp);
		}
	}
	return toReturn;
}

char *printEvent(void *toBePrinted) {
	char *stringReturn;
	char *tempStr;
	int length;
	Event *event;
	event = (Event*)toBePrinted;
	
	char evtUID[256];
	char creationDtDate[256]; 
	char creationDtTime[256]; 
	int creationDtUTC; 
	char startDtDate[256];
	char startDtTime[256]; 
	int startDtUTC; 
	
	strcpy(evtUID, event->UID);
	
	strcpy(creationDtDate, event->creationDateTime.date);
	strcpy(creationDtTime, event->creationDateTime.time);
	creationDtUTC = event->creationDateTime.UTC;
	
	strcpy(startDtDate, event->startDateTime.date);
	strcpy(startDtTime, event->startDateTime.time);
	startDtUTC = event->startDateTime.UTC;

	stringReturn = (char*)malloc(sizeof(char)*(sizeof(event->UID) + sizeof(creationDtDate) + sizeof(creationDtTime) + sizeof(startDtDate) + sizeof(startDtTime + 200)));
    
	sprintf(stringReturn, "Event Components:\n UID = %s\n creationDateTime = %s:%s, UTC=%d\n startDateTime = %s:%s, UTC=%d\n"
    ,evtUID, creationDtDate, creationDtTime, creationDtUTC, startDtDate, startDtTime, startDtUTC);
    
	length = strlen(stringReturn);
    stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + 200));
    
	tempStr = toString(event->properties);
	if(tempStr) {
		strcat(stringReturn, " Event Properties:\n");
		length = strlen(stringReturn);
		stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + strlen(tempStr) + 200));
		strcat(stringReturn, tempStr);
		free(tempStr);
	}
	
    length = strlen(stringReturn);
    strcat(stringReturn, "\n");  
    stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + 200));
  
	tempStr = toString(event->alarms);
	if(tempStr) {
		strcat(stringReturn, "Alarm Components:\n");
		length = strlen(stringReturn);
		stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + strlen(tempStr) + 200));
		strcat(stringReturn, tempStr);
		free(tempStr);
	}

	
	length = strlen(stringReturn);
	stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + 200));

	return stringReturn;
}

void deleteEvent(void *toBeDeleted) {
	Event *event;
  
	//changes type to Event
	event = (Event*)toBeDeleted;
	
	//free linked list of all events
	freeList(event->properties);
	
	freeList(event->alarms);

	free(event);
}

int compareEvents(const void *first, const void *second) {
	Event *event1;
	Event *event2;

	//changes first and second arguments into type event
	event1 = (Event*)first;
	event2 = (Event*)second;

	//return if both UIDs are equal or not
	return strcmp((char*)event1->UID, (char*)event2->UID);
}

char* printAlarm(void *toBePrinted) {
	char *stringReturn;
	char *tempStr;
	int length;
	Alarm *alarm;
	alarm = (Alarm*)toBePrinted;
	
	char alarmAction[256];
	char alarmTrig[256];
	
	strcpy(alarmAction, alarm->action);
	strcpy(alarmTrig, alarm->trigger);

	stringReturn = (char*)malloc(sizeof(char) * (sizeof(alarmAction) + sizeof(alarmTrig) + 200));
	sprintf(stringReturn, "Action: %s\n Trigger: %s\n", alarmAction, alarmTrig);
	length = strlen(stringReturn);
  
	tempStr = toString(alarm->properties);
	if(tempStr) {
		stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + strlen(tempStr) + 200));
		strcat(stringReturn, " Alarm Properties:\n");
		strcat(stringReturn, tempStr);
		length = strlen(stringReturn);
		free(tempStr);
	}


	stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + 200));
	strcat(stringReturn, "\n");

	return stringReturn;
}

void deleteAlarm(void *toBeDeleted) {
	Alarm *alarm;

	//changes type to alarm
	alarm = (Alarm*)toBeDeleted;
	//free linked list
	
	freeList(alarm->properties);

	//if exists, free alarm->trigger
	if(alarm->trigger) {
		free(alarm->trigger);
	}

	free(toBeDeleted);
}

int compareAlarms(const void *first, const void *second) {
	Alarm *alarm1;
	Alarm *alarm2;

	//changes first and second arguments into type alarm
	alarm1 = (Alarm*)first;
	alarm2 = (Alarm*)second;

	//return if equal or not
	return strcmp((char*)alarm1->action, (char*)alarm2->action);
}

char* printProperty(void *toBePrinted) {
	char *stringReturn;
	Property *prop;

	//converts toBePrinted to property
	prop = (Property*)toBePrinted;
	//mallocs the appropriate size to print
	stringReturn = (char*)malloc(sizeof(char) * (strlen(prop->propName) + strlen(prop->propDescr) + 200));
	sprintf(stringReturn, "- %s:%s", prop->propName, prop->propDescr);

	return stringReturn;
}

void deleteProperty(void *toBeDeleted) {
	Property *prop;
	//changes type to Property
	prop = (Property*) toBeDeleted;
	//free property
	free(prop);
}

int compareProperties(const void *first, const void *second) {
	Property *prop1;
	Property *prop2;

	//changes first and second arguments into type property
	prop1 = (Property*)first;
	prop2 = (Property*)second;

	//return if equal or not
	return strcmp((char*)prop1->propName, (char*)prop2->propName);
}

void deleteDate(void* toBeDeleted){
	return;
}
int compareDates(const void* first, const void* second){
	return 0;
}
char* printDate(void* toBePrinted){
	char *stringReturn;
	Event *event;
	event = toBePrinted;

	char creationDtDate[256]; 
	char creationDtTime[256]; 
	int creationDtUTC; 
	char startDtDate[256];
	char startDtTime[256]; 
	int startDtUTC; 
	
	strcpy(creationDtDate, event->creationDateTime.date);
	strcpy(creationDtTime, event->creationDateTime.time);
	creationDtUTC = event->creationDateTime.UTC;
	
	strcpy(startDtDate, event->startDateTime.date);
	strcpy(startDtTime, event->startDateTime.time);
	startDtUTC = event->startDateTime.UTC;

	stringReturn = (char*)malloc(sizeof(char)*(sizeof(creationDtDate) + sizeof(creationDtTime) + sizeof(startDtDate) + sizeof(startDtTime + 200)));
    
	sprintf(stringReturn, "Events:\n creationDateTime = %s:%s, UTC=%d\n startDateTime = %s:%s, UTC=%d\n"
    , creationDtDate, creationDtTime, creationDtUTC, startDtDate, startDtTime, startDtUTC);

	return stringReturn;

}

char* dtToJSON(DateTime prop){
	char *stringReturn;
	char begin[50] = "{\"date\":\"";
	char mid[50] = "\",\"time\":\"";
	char end[50] = "\",\"isUTC\":";
	char bracket[10] = "}";
	
	stringReturn = (char*)malloc(sizeof(char) *(strlen(prop.date) + strlen(prop.date) + 400));
	strcat(begin, prop.date);
	strcpy(stringReturn, begin);

	strcat(stringReturn, mid);	
	strcat(stringReturn, prop.time);
	
	strcat(stringReturn, end);	
	if (prop.UTC == 1){
		strcat(stringReturn, "true");
	}
	else {
		strcat(stringReturn, "false");
	}
	
	strcat(stringReturn, bracket);
	
	return stringReturn;
}

char* eventToJSON(const Event* event){
	char *stringReturn;
	char stringTemp[64] = "";
	char *tempEvent = (char*)event;
	void *data;
	Property *prop;
	char buffer[64];
	char buffer2[64];
	char begin[50] = "{\"startDT\":";
	char mid[50] = ",\"numProps\":";
	char mid2[50] = ",\"numAlarms\":";
	char mid3[50] = ",\"summary\":\"";
	char end[50] = "\"}";
	char nullEvent[50] = "{}";
	Event *temp = (Event*)event;
	char *dtJSFunc = dtToJSON(temp->startDateTime);	
	ListIterator iter = createIterator(event->properties);
	ListIterator subIter = createIterator(event->alarms);
	
	if(tempEvent == NULL){
		stringReturn = (char*)malloc(sizeof(char));
		strcpy(stringReturn, nullEvent);
		return stringReturn;
	}
	
	int numProps = 0;
	int numAlarms = 0;
	
	if(event->properties != NULL){
		while(iter.current != NULL){
			data = nextElement(&iter);
			prop = (Property*)data;
			if(strcmp(prop->propName, "SUMMARY") == 0){
				strcpy(stringTemp, prop->propDescr);
				numProps++;
			}
			else{
				strcpy(stringTemp, "");
				numProps++;
			}
		}
		numProps = numProps + 3;
	}
	
	if(event->alarms != NULL){
		while(subIter.current != NULL){
			numAlarms++;
			nextElement(&subIter);
		}
	}
	
	snprintf(buffer, sizeof buffer, "%d", numProps);
	snprintf(buffer2, sizeof buffer2, "%d", numAlarms);
	
	stringReturn = (char*)malloc(sizeof(char) + 200);
	
	strcat(begin, dtJSFunc);
	strcpy(stringReturn, begin);
	
	strcat(stringReturn, mid);
	strcat(stringReturn, buffer);
	
	strcat(stringReturn, mid2);
	strcat(stringReturn, buffer2);
	
	strcat(stringReturn, mid3);
	
	strcat(stringReturn, stringTemp);
	
	free(dtJSFunc);
	strcat(stringReturn, end);
	
	//printf("eventToJSON Test: %s\n", stringReturn);
	
	return stringReturn;
}

char* eventListToJSON(const List* eventList){
	char *stringReturn;
	List* temp = (List*)eventList;
	char* temp2 = (char*)eventList;
	Event *event = (Event*)malloc(sizeof(Event));
	ListIterator iter;
	void *data;
	char begin[256] = "[";
	char mid[256] = ",";
	char end[256] = "]";
	int length = 0;
	char nullStr[256] = "[]";
	
	stringReturn = (char*)malloc(sizeof(char) + 200);
	
	if(eventList == NULL || strcmp(temp2, "") == 0){
		strcpy(stringReturn, nullStr);
		return stringReturn;
	}
	
	strcpy(stringReturn, begin);
	
	iter = createIterator(temp);
	while(iter.current != NULL){
		data = nextElement(&iter);
		event = (Event*)data;
		char *evtJSFunc = eventToJSON(event);
		//printf("Evt JS Func Test: %s\n", evtJSFunc);
		strcat(stringReturn, evtJSFunc);
		if(iter.current == NULL){
			strcat(stringReturn, end);
		}
		else {
			strcat(stringReturn, mid);
		}
		free(evtJSFunc);
		length = strlen(stringReturn);
		stringReturn = (char*)realloc(stringReturn, sizeof(char) * (length + 200));
	}
	
	return stringReturn;
}

char* calendarToJSON(const Calendar* cal){
	char *stringReturn;	
	char begin[50] = "{\"version\":";
	char mid[50] = ",\"prodID\":\"";
	char mid2[50] = "\",\"numProps\":";
	char mid3[50] = ",\"numEvents\":";
	char end[50] = "}";
	char *tempCal = (char*)cal;
	char buffer[64];
	char buffer2[64];
	char buffer3[64];
	int propVal = 0;
	int numEvents = 0;
	ListIterator iter = createIterator(cal->properties);
	ListIterator subIter = createIterator(cal->events);
	
	stringReturn = (char*)malloc(sizeof(char) * (strlen(cal->prodID) + 200));
	
	if(tempCal == NULL){
		strcpy(stringReturn, "{}");
		return stringReturn;
	}
	
	if(cal->events == NULL){
		stringReturn = (char*)malloc(sizeof(char) * (strlen(cal->prodID) + 200));
		strcpy(stringReturn, "{}");
		return stringReturn;
	}
	
	if(cal->properties != NULL){
		while(iter.current != NULL){
			nextElement(&iter);
			propVal++;
		}	
	propVal = propVal + 2;
	}
	
	if(cal->events !=NULL){
		while(subIter.current != NULL){
			nextElement(&subIter);
			numEvents++;
		}
	}

	snprintf(buffer, sizeof buffer,"%.f",cal->version);
	snprintf(buffer2, sizeof buffer2,"%d",propVal);
	snprintf(buffer3, sizeof buffer3,"%d",numEvents);
	
	strcat(begin, buffer);
	strcpy(stringReturn, begin);
	
	strcat(stringReturn, mid);
	strcat(stringReturn, cal->prodID);
	
	strcat(stringReturn, mid2);
	strcat(stringReturn, buffer2);
	
	strcat(stringReturn, mid3);
	strcat(stringReturn, buffer3);
	strcat(stringReturn, end);
	
	return stringReturn;
}

Calendar* JSONtoCalendar(const char* str){
	Calendar *tempCal = NULL;
	tempCal = (Calendar*)malloc(sizeof(Calendar));
	tempCal->version = 0.0;
	char line[256];
	char *subString;
	
	if(str == NULL){
		return NULL;
	}
	
	if(!str){
		return NULL;
	}
	
	tempCal->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
	tempCal->events->length = 0;
	
	tempCal->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
	tempCal->properties->length = 0;
	
	strcpy(line, str);

	subString = strtok(line,"{version\":.,\"");
	//printf("The line is %s\n", line);
	tempCal->version = atof(subString);
	printf("VERSION: %.f\n",tempCal->version);
	
	subString = strtok(NULL, "\"");
	//printf("The line is %s\n", subString);
	subString = strtok(NULL, "\":\"");
	//printf("The line is %s\n", subString);
	subString = strtok(NULL, "\":\"");
	printf("The line is %s\n", subString);
	//strcpy(tempCal->prodID, subString);
	subString = strtok(NULL, "\":\"");
	//subString = strtok(NULL, "\":\"");
	strcpy(tempCal->prodID, subString);
	printf("PRODID: %s\n", tempCal->prodID);
	
	return tempCal;
}

Event* JSONtoEvent(const char* str){
	Event* tempEvent = NULL;
	tempEvent = (Event*)malloc(sizeof(Event));
	char line[256];
	char *subString;
	
	if(str == NULL){
		return NULL;
	}
	
	if(!str){
		return NULL;
	}
	
	strcpy(line, str);
	
	tempEvent->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
	tempEvent->properties->length = 0;
	
	tempEvent->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);
	tempEvent->alarms->length = 0;
	
	subString = strtok(line, "{\"UID\":\"");
	strcpy(tempEvent->UID, subString);
	printf("UID: %s\n", tempEvent->UID);
	
	strcmp(tempEvent->creationDateTime.date, "");
	strcmp(tempEvent->creationDateTime.time, "");
	tempEvent->creationDateTime.UTC = false;
	
	strcmp(tempEvent->startDateTime.date, "");
	strcmp(tempEvent->startDateTime.time, "");
	tempEvent->startDateTime.UTC = false;

	return tempEvent;
}

void addEvent(Calendar* cal, Event* toBeAdded){
	
	if(cal == NULL){
		return;
	}
	if(toBeAdded == NULL){
		return;
	}
	
	insertBack(cal->events, toBeAdded);
	
	return;
}

char* printError(ICalErrorCode err){
	char *errStr;
	char *errReturn;

	//case swithces of all the possible errors
	switch(err){
		case OK:
			errStr = "OK";
			break;
		case INV_FILE:
			errStr = "Invalid File";
			break;
		case INV_CAL:
			errStr = "Invalid Calendar";
			break;
		case INV_VER:
			errStr = "Invalid Version";
			break;
		case DUP_VER:
			errStr = "Multiple Versions Error";
			break;
		case INV_PRODID:
			errStr = "Invalid Product ID";
			break;
		case DUP_PRODID:
			errStr = "Multiple Product ID Error";
			break;
		case INV_EVENT:
			errStr = "Invalid Event";
			break;
		case INV_DT:
			errStr = "Invalid Date Time";
			break;
		case INV_ALARM:
			errStr = "Invalid Alarm";
			break;
		case OTHER_ERROR:
			errStr = "Unknown Error";
			break;
		default:
			errStr = "Unknown Error";
			break;
	}

	errReturn = malloc((strlen(errStr) + 100) * sizeof(char));
	strcpy(errReturn, errStr);

	return errReturn;
}

void unfoldLine(char *lineClear) {
	char *firstChar;
	size_t lineLen;
	
	firstChar = lineClear;

	lineLen = strlen(firstChar) + 1;
	memmove(lineClear, firstChar, lineLen);

	if(lineClear[strlen(lineClear) - 2] == '\r' && lineClear[strlen(lineClear) - 1] == '\n') {
		lineClear[strlen(lineClear) - 2] = '\0';
	} 
	else if(lineClear[strlen(lineClear) - 1] == '\r' || lineClear[strlen(lineClear) - 1] == '\n') {
		lineClear[strlen(lineClear) - 1] = '\0';
	}	
}

ICalErrorCode errorCode(Calendar *cal, FILE *file, Calendar **obj, ICalErrorCode err) {
	printf("ERROR!!!!!\n");
	if(file) {
		fclose(file);
	}

	obj = NULL;
	deleteCalendar(cal);
	return err;
}

bool compareProperty(const void *first, const void *second) {
	Property *prop = (Property*)first;
	char *title = (char*)second;
  
	if(strcmp((char*)prop->propName, title) == 0) {
		return true;
	} 
	else{
		return false;
	}
}

bool diffProperty(const void *first, const void *second) {
	Property *prop1 = (Property*)first;
	Property *prop2 = (Property*)second;
  
	return (strcmp((char*)prop1->propName, (char*)prop2->propName) == 0
    && strcmp((char*)prop1->propDescr, (char*)prop2->propDescr) != 0);
}
