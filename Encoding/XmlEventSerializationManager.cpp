#include "XmlEventSerializationManager.h"

#include "events/NetworkTrafficReceivedEvent.h"
#include "file/tinyxml2.h"
#include "net/Fish.h"
#include "net/PingMessage.h"

namespace gamelib
{
	std::shared_ptr<PlayerMovedEvent> XmlEventSerializationManager::DeserializePlayerMovedEvent(
		std::string serializedMessage)
	{
		return json.DeserializePlayerMovedEvent(serializedMessage);
	}

	std::string XmlEventSerializationManager::SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object,
		std::string target)
	{
		return json.SerializePlayerMovedEvent(object, target);
	}

	std::string XmlEventSerializationManager::CreateRequestPlayerDetailsMessage()
	{
		return json.CreateRequestPlayerDetailsMessage();
	}

	std::string XmlEventSerializationManager::CreatePongMessage()
	{
		return CreatePingMessage();
	}

	std::string XmlEventSerializationManager::CreateDummyXml()
	{
		/*
		 <Root>
		    <IntValue>10</IntValue>
		    <FloatValue>0.5</FloatValue>
		    <Date day="26" month="April" year="2014" dateFormat="26/04/2014"/>
		    <List itemCount="8">
		        <Item>1</Item>
		        <Item>1</Item>
		        <Item>2</Item>
		        <Item>3</Item>
		        <Item>5</Item>
		        <Item>8</Item>
		        <Item>13</Item>
		        <Item>21</Item>
		    </List>
		</Root>
		 */

		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("Root");		
		xmlDoc.InsertFirstChild(pRoot);

		tinyxml2::XMLElement * pElement = xmlDoc.NewElement("IntValue");
		pElement->SetText(10);
		pRoot->InsertEndChild(pElement);

		pElement = xmlDoc.NewElement("FloatValue");
		pElement->SetText(0.5f);
		pRoot->InsertEndChild(pElement);

		pElement = xmlDoc.NewElement("Date");
		pElement->SetAttribute("day", 26);
		pElement->SetAttribute("month", "April");
		pElement->SetAttribute("year", 2014);
		pElement->SetAttribute("dateFormat", "26/04/2014");
		pRoot->InsertEndChild(pElement);

		const std::vector vecList  { 1, 2, 3, 4, 5};

		pElement = xmlDoc.NewElement("List");
		pElement->SetAttribute("itemCount", "5");
		for (const auto & item : vecList)
		{
			tinyxml2::XMLElement * pListElement = xmlDoc.NewElement("Item");
			pListElement->SetText(item);

			pElement->InsertEndChild(pListElement);
		}

		pRoot->InsertEndChild(pElement);

		tinyxml2::XMLPrinter printer;
		xmlDoc.Print(&printer);
		auto xml = printer.CStr();
		return xml;
	}

	std::string XmlEventSerializationManager::CreatePingMessage()
	{
		/*
		 Create this XML format:
		 
		 <PingMessage>
		    <Type>ping</Type>
		    <IsHappy>true</IsHappy>
		    <EventType>1015</EventType>
		    <Names itemCount="3">
		        <Name>Stuart</Name>
		        <Name>Jenny</Name>
		        <Name>bruce</Name>
		    </Names>
		    <Ages itemCount="3">
		        <Age>1</Age>
		        <Age>2</Age>
		        <Age>3</Age>
		    </Ages>
		    <Fish Name="Neemo" Surname="Mathews"/>
		</PingMessage>

		 */

		PingMessage pingMessage;
			pingMessage.Type = "ping";
			pingMessage.IsHappy = true;
			pingMessage.EventType = NetworkTrafficReceivedEventId.PrimaryId;
			pingMessage.Names = { "Stuart", "Jenny", "bruce" };
			pingMessage.Ages = {1, 2,3 };
			pingMessage.Fish = Fish("Neemo", "Mathews");

		// <PingMessage>
		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("PingMessage");

		xmlDoc.InsertFirstChild(pRoot);

		// <Type>
		tinyxml2::XMLElement * pElement = xmlDoc.NewElement("Type");
		pElement->SetText(pingMessage.Type.c_str());
		pRoot->InsertEndChild(pElement);

		// <IsHappy>
		pElement = xmlDoc.NewElement("IsHappy");
		pElement->SetText(pingMessage.IsHappy);
		pRoot->InsertEndChild(pElement);

		// <EventType>
		pElement = xmlDoc.NewElement("EventType");
		pElement->SetText(pingMessage.EventType);
		pRoot->InsertEndChild(pElement);

		/*
		 <Names>
		  <Name><Stuart/Name>
		  <Name><Jenny/Name>
		  <Name><Bruce/Name>
		 </Names>
		 */

		pElement = xmlDoc.NewElement("Names");
		pElement->SetAttribute("itemCount", "3");
		for (const auto & item : pingMessage.Names )
		{
			tinyxml2::XMLElement * pListElement = xmlDoc.NewElement("Name");
			pListElement->SetText(item.c_str());

			pElement->InsertEndChild(pListElement);
		}
		pRoot->InsertEndChild(pElement);

		/*
		 <Ages>
		  <Age><1/Age>
		  <Age><2/Age>
		  <Age><3/Age>
		 </Names>
		 */
		pElement = xmlDoc.NewElement("Ages");
		pElement->SetAttribute("itemCount", "3");
		for (const auto & item : pingMessage.Ages )
		{
			tinyxml2::XMLElement * pListElement = xmlDoc.NewElement("Age");
			pListElement->SetText(item);

			pElement->InsertEndChild(pListElement);
		}
		pRoot->InsertEndChild(pElement);

		// <Fish Name="Neemo" Surname="Mathews" />
		pElement = xmlDoc.NewElement("Fish");
		pElement->SetAttribute("Name", pingMessage.Fish.Name.c_str());
		pElement->SetAttribute("Surname", pingMessage.Fish.Surname.c_str());
		pRoot->InsertEndChild(pElement);

		tinyxml2::XMLPrinter printer;
		xmlDoc.Print(&printer);
		auto xml = printer.CStr();
		return xml;
	}

	std::string XmlEventSerializationManager::SerializeControllerMoveEvent(
		std::shared_ptr<ControllerMoveEvent> object, std::string target)
	{
		return json.SerializeControllerMoveEvent(object, target);
	}

	std::string XmlEventSerializationManager::CreateUnknownEventMessage(std::shared_ptr<Event> evt,
		std::string target)
	{
		return json.CreateUnknownEventMessage(evt, target);
	}

	std::string XmlEventSerializationManager::SerializeStartNetworkLevelEvent(
		std::shared_ptr<StartNetworkLevelEvent> evt, std::string target)
	{
		return json.SerializeStartNetworkLevelEvent(evt, target);
	}

	std::shared_ptr<StartNetworkLevelEvent> XmlEventSerializationManager::DeserializeStartNetworkLevel(
		std::string serializedMessage)
	{
		return json.DeserializeStartNetworkLevel(serializedMessage);
	}

	std::string XmlEventSerializationManager::CreateRequestPlayerDetailsMessageResponse(const std::string& target)
	{
		return json.CreateRequestPlayerDetailsMessageResponse(target);
	}
}
