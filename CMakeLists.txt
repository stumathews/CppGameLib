cmake_minimum_required(VERSION 3.27)

project(cppgamelib VERSION 1.0)

# Use the latest C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the library using the library source files
add_library(cppgamelib STATIC 
ai/ActiveSelector.h
ai/Behavior.h
ai/BehavioralSequence.h
ai/BehaviorResult.h
ai/BehaviorTree.cpp
ai/BehaviorTree.h
ai/BehaviorTreeBuilder.cpp
ai/BehaviorTreeBuilder.h
ai/CompositeBehavior.h
ai/ConditionalSequence.h
ai/Decorator.h
ai/FilterSequence.h
ai/FSM.cpp
ai/FSM.h
ai/FSMState.cpp
ai/FSMState.h
ai/FSMTransition.h
ai/InlineBehavioralAction.h
ai/Monitor.h
ai/Parallel.h
ai/Repeat.h
ai/Selector.h
asset/asset.cpp
asset/asset.h
asset/SpriteAsset.cpp
asset/SpriteAsset.h
audio/AudioAsset.cpp
audio/AudioAsset.h
audio/AudioManager.cpp
audio/AudioManager.h
character/AnimatedSprite.cpp
character/AnimatedSprite.h
character/Component.cpp
character/Component.h
character/Direction.h
character/DirectionUtils.cpp
character/DirectionUtils.h
character/Hotspot.cpp
character/Hotspot.h
character/IGameMoveStrategy.h
character/IMovement.h
character/Inventory.cpp
character/Inventory.h
character/Movement.h
character/MovementAtSpeed.cpp
character/MovementAtSpeed.h
character/Npc.cpp
character/Npc.h
character/StatefulMove.cpp
character/StatefulMove.h
character/StaticSprite.cpp
character/StaticSprite.h
common/aliases.h
common/Common.h
common/constants.h
common/TypeAliases.h
cppgamelib/ai.h
cppgamelib/asset.h
cppgamelib/audio.h
cppgamelib/character.h
cppgamelib/events.h
cppgamelib/exceptions.h
cppgamelib/file.h
cppgamelib/font.h
cppgamelib/geometry.h
cppgamelib/graphic.h
cppgamelib/json.h
cppgamelib/logging.h
cppgamelib/net.h
cppgamelib/objects.h
cppgamelib/processes.h
cppgamelib/resource.h
cppgamelib/scene.h
cppgamelib/time.h
cppgamelib/utils.h
cppgamelib.cpp
cppgamelib.h
Encoding/BitPackedEventSerializationManager.cpp
Encoding/BitPackedEventSerializationManager.h
Encoding/JsonEventSerializationManager.cpp
Encoding/JsonEventSerializationManager.h
Encoding/XmlEventSerializationManager.cpp
Encoding/XmlEventSerializationManager.h
events/AddGameObjectToCurrentSceneEvent.cpp
events/AddGameObjectToCurrentSceneEvent.h
events/ControllerMoveEvent.cpp
events/ControllerMoveEvent.h
events/Event.cpp
events/Event.h
events/EventFactory.cpp
events/EventFactory.h
events/EventId.cpp
events/EventId.h
events/EventManager.cpp
events/EventManager.h
events/EventNumbers.h
events/EventSubscriber.cpp
events/EventSubscriber.h
events/GameObjectEvent.cpp
events/GameObjectEvent.h
events/IEventSerializationManager.cpp
events/IEventSerializationManager.h
events/IEventSubscriber.cpp
events/IEventSubscriber.h
events/NetworkPlayerJoinedEvent.h
events/NetworkTrafficReceivedEvent.cpp
events/NetworkTrafficReceivedEvent.h
events/PlayerMovedEvent.cpp
events/PlayerMovedEvent.h
events/ReliableUdpAckPacketEvent.h
events/ReliableUdpCheckSumFailedEvent.h
events/ReliableUdpPacketLossDetectedEvent.h
events/ReliableUdpPacketReceivedEvent.cpp
events/ReliableUdpPacketReceivedEvent.h
events/ReliableUdpPacketRttCalculatedEvent.h
events/SceneChangedEvent.cpp
events/SceneChangedEvent.h
events/SceneLoadedEvent.cpp
events/SceneLoadedEvent.h
events/StartNetworkLevelEvent.h
events/SubscriberHandledEvent.h
events/UpdateAllGameObjectsEvent.cpp
events/UpdateAllGameObjectsEvent.h
events/UpdateProcessesEvent.h
exceptions/EngineException.cpp
exceptions/EngineException.h
file/Logger.cpp
file/Logger.h
file/SerializationManager.cpp
file/SerializationManager.h
file/SettingDetail.cpp
file/SettingDetail.h
file/SettingsManager.cpp
file/SettingsManager.h
file/TextFile.cpp
file/TextFile.h
file/tinyxml2.cpp
file/tinyxml2.h
font/FontAsset.cpp
font/FontAsset.h
font/FontManager.cpp
font/FontManager.h
framework.h
geometry/ABCDRectangle.cpp
geometry/ABCDRectangle.h
geometry/Coordinate.cpp
geometry/Coordinate.h
geometry/Line.cpp
geometry/Line.h
geometry/Side.h
geometry/SideUtils.h
graphic/ColourKey.cpp
graphic/ColourKey.h
graphic/DrawableFrameRate.cpp
graphic/DrawableFrameRate.h
graphic/DrawableText.cpp
graphic/DrawableText.h
graphic/Drawing.cpp
graphic/Drawing.h
graphic/GraphicAsset.cpp
graphic/GraphicAsset.h
graphic/GraphicAssetFactory.cpp
graphic/GraphicAssetFactory.h
graphic/KeyFrame.cpp
graphic/KeyFrame.h
graphic/RectDebugging.h
graphic/SDLGraphicsManager.cpp
graphic/SDLGraphicsManager.h
graphic/Subscribable.cpp
graphic/Subscribable.h
graphic/Window.cpp
graphic/Window.h
input/IInputManager.cpp
input/IInputManager.h
json/json11.cpp
json/json11.h
Logging/ErrorLogManager.cpp
Logging/ErrorLogManager.h
net/IElapsedTimeProvider.h
net/BitfieldReader.h
net/BitPacker.h
net/BitPackingTypes.h
net/crc.h
net/Fish.h
net/GameClient.cpp
net/GameClient.h
net/GameClientConnectionFactory.cpp
net/GameClientConnectionFactory.h
net/GameServer.cpp
net/GameServer.h
net/GameServerConnectionFactory.h
net/GameStatePusher.cpp
net/GameStatePusher.h
net/IConnectedNetworkSocket.h
net/IGameServerConnection.h
net/IProtocolManager.h
net/Message.cpp
net/Message.h
net/MessageHeader.h
net/Networking.cpp
net/Networking.h
net/NetworkingActivityMonitor.cpp
net/NetworkingActivityMonitor.h
net/NetworkingStatistics.h
net/NetworkManager.cpp
net/NetworkManager.h
net/NetworkPlayer.cpp
net/NetworkPlayer.h
net/PacketDatum.cpp
net/PacketDatum.h
net/PacketDatumUtils.h
net/PeerInfo.cpp
net/PeerInfo.h
net/PingMessage.h
net/PongMessage.h
net/ReliableUdp.cpp
net/ReliableUdp.h
net/ReliableUdpGameServerConnection.cpp
net/ReliableUdpGameServerConnection.h
net/ReliableUdpMessageHeader.cpp
net/ReliableUdpMessageHeader.h
net/ReliableUdpProtocolManager.cpp
net/ReliableUdpProtocolManager.h
net/Rtt.h
net/TcpGameServerConnection.cpp
net/TcpGameServerConnection.h
net/TcpNetworkPlayer.cpp
net/TcpNetworkPlayer.h
net/TcpNetworkSocket.cpp
net/TcpNetworkSocket.h
net/TransportOnlyProtocolManager.h
net/UdpConnectedNetworkSocket.cpp
net/UdpConnectedNetworkSocket.h
net/UdpGameServerConnection.cpp
net/UdpGameServerConnection.h
net/UdpNetworkPlayer.cpp
net/UdpNetworkPlayer.h
objects/DrawableGameObject.cpp
objects/DrawableGameObject.h
objects/GameObject.cpp
objects/GameObject.h
objects/GameObjectFactory.cpp
objects/GameObjectFactory.h
objects/GameObjectType.h
objects/GameWorldData.h
objects/game_world_component.h
pch.cpp
pch.h
processes/Action.h
processes/DelayProcess.h
processes/Process.h
processes/ProcessManager.cpp
processes/ProcessManager.h
resource/ResourceManager.cpp
resource/ResourceManager.h
scene/layer.cpp
scene/Layer.h
scene/SceneManager.cpp
scene/SceneManager.h
security/Security.cpp
security/Security.h
structure/FixedStepGameLoop.cpp
structure/FixedStepGameLoop.h
structure/GameStructure.cpp
structure/GameStructure.h
structure/IGameLoopStrategy.h
structure/Profiler.cpp
structure/Profiler.h
structure/VariableGameLoop.cpp
structure/VariableGameLoop.h
time/PeriodicTimer.cpp
time/PeriodicTimer.h
time/Timer.cpp
time/Timer.h
utils/BitFiddler.h
utils/RingBuffer.h
utils/Statistics.h
utils/Utils.h
)

# Find packages for library dependencies (these will come via vcpkg)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)

# Set a few properties for the library 
set_target_properties(cppgamelib PROPERTIES LINKER_LANGUAGE CXX)

# Link to external dependencies
target_link_libraries(cppgamelib 
SDL2::SDL2 
SDL2_ttf::SDL2_ttf
SDL2_mixer::SDL2_mixer
SDL2_image::SDL2_image
unofficial-sodium::sodium)

# Set the libraries include 
target_include_directories(cppgamelib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Make an executable that runs all the tests the the provided files
enable_testing()

# Add an executable for running all tests. This excludes networking tests
add_executable(AllTests
Tests/Tests/BehaviorTreeBuilderTests.cpp 
Tests/Tests/BehaviorTreeTests.cpp 
Tests/Tests/BitFiddlerTests.cpp 
Tests/Tests/BitPackerTests.cpp
Tests/Tests/CrcTests.cpp
Tests/Tests/FSMTests.cpp
Tests/Tests/RingBufferTests.cpp
Tests/Tests/EventManagerTests.cpp
Tests/Tests/pch.cpp
Tests/Tests/ReliableUdpTests.cpp
Tests/Tests/SecurityTests.cpp
Tests/Tests/SerializationTests.cpp
Tests/Tests/SettingsManagerTests.cpp
Tests/Tests/StatisticsTests.cpp
Tests/Tests/ResourceManagerTests.cpp
Tests/Tests/AudioManagerTests.cpp 
Tests/Tests/FontManagerTests.cpp
Tests/Tests/GraphicAssetFactoryTests.cpp
Tests/Tests/SceneGraphTests.cpp
Tests/Tests/SceneManagerTests.cpp
Tests/Tests/SDLGraphicsManagerTests.cpp
)

# Add an executable for running only the networking tests
add_executable(NetworkingTests
Tests/Tests/NetworkingTests.cpp
)

# Set the properties for the test executables
set_target_properties(AllTests PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(NetworkingTests PROPERTIES LINKER_LANGUAGE CXX)

# Set the include directories for the test executables
target_include_directories(AllTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Tests/Tests)
target_include_directories(NetworkingTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Tests/Tests)

# Link libraries that the test AllTests exeutable uses
target_link_libraries(AllTests PUBLIC 
cppgamelib
SDL2::SDL2
SDL2_ttf::SDL2_ttf
SDL2_mixer::SDL2_mixer
SDL2_image::SDL2_image
PRIVATE
GTest::gmock_main winmm)

# Set include directory for allTests executable
target_include_directories(AllTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries that the test AllTests exeutable uses
target_link_libraries(NetworkingTests PUBLIC 
cppgamelib
unofficial-sodium::sodium PRIVATE
GTest::gmock_main winmm)

target_include_directories(NetworkingTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS cppgamelib DESTINATION lib)

