cmake_minimum_required(VERSION 4.0)

project(cppgamelib VERSION 1.0)

include(GenerateExportHeader)

# Include functionality that will help us create the package config for this project
include(CMakePackageConfigHelpers)

enable_testing()

# Use the latest C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all the header files into a variable called headerFiles.
# This will make it easier to reference or update this list in the future.

set(headerFiles
ai/ActiveSelector.h
ai/Behavior.h
ai/BehavioralSequence.h
ai/BehaviorResult.h
ai/BehaviorTree.h
ai/BehaviorTreeBuilder.h
ai/CompositeBehavior.h
ai/ConditionalSequence.h
ai/Decorator.h
ai/FilterSequence.h
ai/FSM.h
ai/FSMState.h
ai/FSMTransition.h
ai/InlineBehavioralAction.h
ai/Monitor.h
ai/Parallel.h
ai/Repeat.h
ai/Selector.h
asset/asset.h
asset/SpriteAsset.h
audio/AudioAsset.h
audio/AudioManager.h
character/AnimatedSprite.h
character/Component.h
character/Direction.h
character/DirectionUtils.h
character/Hotspot.h
character/IGameMoveStrategy.h
character/IMovement.h
character/Inventory.h
character/Movement.h
character/MovementAtSpeed.h
character/Npc.h
character/StatefulMove.h
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
cppgamelib.h
Encoding/BitPackedEventSerializationManager.h
Encoding/JsonEventSerializationManager.h
Encoding/XmlEventSerializationManager.h
events/AddGameObjectToCurrentSceneEvent.h
events/ControllerMoveEvent.h
events/Event.h
events/EventFactory.h
events/EventId.h
events/EventManager.h
events/EventNumbers.h
events/EventSubscriber.h
events/GameObjectEvent.h
events/IEventSerializationManager.h
events/IEventSubscriber.h
events/NetworkPlayerJoinedEvent.h
events/NetworkTrafficReceivedEvent.h
events/PlayerMovedEvent.h
events/ReliableUdpAckPacketEvent.h
events/ReliableUdpCheckSumFailedEvent.h
events/ReliableUdpPacketLossDetectedEvent.h
events/ReliableUdpPacketReceivedEvent.h
events/ReliableUdpPacketRttCalculatedEvent.h
events/SceneChangedEvent.h
events/SceneLoadedEvent.h
events/StartNetworkLevelEvent.h
events/SubscriberHandledEvent.h
events/UpdateAllGameObjectsEvent.h
events/UpdateProcessesEvent.h
exceptions/EngineException.h
file/Logger.h
file/SerializationManager.h
file/SettingDetail.h
file/SettingsManager.h
file/TextFile.h
file/tinyxml2.h
font/FontAsset.h
font/FontManager.h
framework.h
geometry/ABCDRectangle.h
geometry/Coordinate.h
geometry/Line.h
geometry/Side.h
geometry/SideUtils.h
graphic/ColourKey.h
graphic/DrawableFrameRate.h
graphic/DrawableText.h
graphic/Drawing.h
graphic/GraphicAsset.h
graphic/GraphicAssetFactory.h
graphic/KeyFrame.h
graphic/RectDebugging.h
graphic/SDLGraphicsManager.h
graphic/Subscribable.h
graphic/Window.h
input/IInputManager.h
json/json11.h
Logging/ErrorLogManager.h
net/IElapsedTimeProvider.h
net/BitfieldReader.h
net/BitPacker.h
net/BitPackingTypes.h
net/crc.h
net/Fish.h
net/GameClient.h
net/GameClientConnectionFactory.h
net/GameServer.h
net/GameServerConnectionFactory.h
net/GameStatePusher.h
net/IConnectedNetworkSocket.h
net/IGameServerConnection.h
net/IProtocolManager.h
net/Message.h
net/MessageHeader.h
net/Networking.h
net/NetworkingActivityMonitor.h
net/NetworkingStatistics.h
net/NetworkManager.h
net/NetworkPlayer.h
net/PacketDatum.h
net/PacketDatumUtils.h
net/PeerInfo.h
net/PingMessage.h
net/PongMessage.h
net/ReliableUdp.h
net/ReliableUdpGameServerConnection.h
net/ReliableUdpMessageHeader.h
net/ReliableUdpProtocolManager.h
net/Rtt.h
net/TcpGameServerConnection.h
net/TcpNetworkPlayer.h
net/TcpNetworkSocket.h
net/TransportOnlyProtocolManager.h
net/UdpConnectedNetworkSocket.h
net/UdpGameServerConnection.h
net/UdpNetworkPlayer.h
objects/DrawableGameObject.h
objects/GameObject.h
objects/GameObjectFactory.h
objects/GameObjectType.h
objects/GameWorldData.h
objects/game_world_component.h
pch.h
processes/Action.h
processes/DelayProcess.h
processes/Process.h
processes/ProcessManager.h
resource/ResourceManager.h
scene/Layer.h
scene/SceneManager.h
security/Security.h
structure/FixedStepGameLoop.h
structure/GameStructure.h
structure/IGameLoopStrategy.h
structure/Profiler.h
structure/VariableGameLoop.h
time/PeriodicTimer.h
time/Timer.h
utils/BitFiddler.h
utils/RingBuffer.h
utils/Statistics.h
utils/Utils.h
)

# Collect all the source files into a variable called sourceFiles
# This will make updating referencing and updating this list easier in the future.

set(sourceFiles
ai/BehaviorTree.cpp
ai/BehaviorTreeBuilder.cpp
ai/FSM.cpp
ai/FSMState.cpp
asset/asset.cpp
asset/SpriteAsset.cpp
audio/AudioAsset.cpp
audio/AudioManager.cpp
character/AnimatedSprite.cpp
character/Component.cpp
character/DirectionUtils.cpp
character/Hotspot.cpp
character/Inventory.cpp
character/MovementAtSpeed.cpp
character/Npc.cpp
character/StatefulMove.cpp
character/StaticSprite.cpp
cppgamelib.cpp
Encoding/BitPackedEventSerializationManager.cpp
Encoding/JsonEventSerializationManager.cpp
Encoding/XmlEventSerializationManager.cpp
events/AddGameObjectToCurrentSceneEvent.cpp
events/ControllerMoveEvent.cpp
events/Event.cpp
events/EventFactory.cpp
events/EventId.cpp
events/EventManager.cpp
events/EventSubscriber.cpp
events/GameObjectEvent.cpp
events/IEventSerializationManager.cpp
events/IEventSubscriber.cpp
events/NetworkTrafficReceivedEvent.cpp
events/PlayerMovedEvent.cpp
events/ReliableUdpPacketReceivedEvent.cpp
events/SceneChangedEvent.cpp
events/SceneLoadedEvent.cpp
events/UpdateAllGameObjectsEvent.cpp
exceptions/EngineException.cpp
file/Logger.cpp
file/SerializationManager.cpp
file/SettingDetail.cpp
file/SettingsManager.cpp
file/TextFile.cpp
file/tinyxml2.cpp
font/FontAsset.cpp
font/FontManager.cpp
geometry/ABCDRectangle.cpp
geometry/Coordinate.cpp
geometry/Line.cpp
graphic/ColourKey.cpp
graphic/DrawableFrameRate.cpp
graphic/DrawableText.cpp
graphic/Drawing.cpp
graphic/GraphicAsset.cpp
graphic/GraphicAssetFactory.cpp
graphic/KeyFrame.cpp
graphic/SDLGraphicsManager.cpp
graphic/Subscribable.cpp
graphic/Window.cpp
input/IInputManager.cpp
json/json11.cpp
Logging/ErrorLogManager.cpp
net/GameClient.cpp
net/GameClientConnectionFactory.cpp
net/GameServer.cpp
net/GameStatePusher.cpp
net/Message.cpp
net/Networking.cpp
net/NetworkingActivityMonitor.cpp
net/NetworkManager.cpp
net/NetworkPlayer.cpp
net/PacketDatum.cpp
net/PeerInfo.cpp
net/ReliableUdp.cpp
net/ReliableUdpGameServerConnection.cpp
net/ReliableUdpMessageHeader.cpp
net/ReliableUdpProtocolManager.cpp
net/TcpGameServerConnection.cpp
net/TcpNetworkPlayer.cpp
net/TcpNetworkSocket.cpp
net/UdpConnectedNetworkSocket.cpp
net/UdpGameServerConnection.cpp
net/UdpNetworkPlayer.cpp
objects/DrawableGameObject.cpp
objects/GameObject.cpp
objects/GameObjectFactory.cpp
pch.cpp
processes/ProcessManager.cpp
resource/ResourceManager.cpp
scene/layer.cpp
scene/SceneManager.cpp
security/Security.cpp
structure/FixedStepGameLoop.cpp
structure/GameStructure.cpp
structure/Profiler.cpp
structure/VariableGameLoop.cpp
time/PeriodicTimer.cpp
time/Timer.cpp
)

# Combine the header and source files int a variable called sourceAndHeaderFiles

set(sourceAndHeaderFiles ${headerFiles})
list (APPEND sourceAndHeaderFiles ${sourceFiles})

# Create the library using the library source files
add_library(cppgamelib STATIC ${sourceAndHeaderFiles})

target_include_directories(cppgamelib
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_sources(
cppgamelib 
PUBLIC 
 FILE_SET public_headers
 TYPE HEADERS
 FILES ${headerFiles}
)

add_library(cppgamelib::cppgamelib ALIAS cppgamelib)

# Generate a header file containing preprocessor macro definitions to control C/C++ symbol visibility.
generate_export_header(cppgamelib)

# Find packages for library dependencies (these will come via vcpkg)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)

# Set a few properties for the library 
set_target_properties(
cppgamelib 
PROPERTIES 
LINKER_LANGUAGE CXX 
)

# Link to external dependencies
target_link_libraries(cppgamelib 
SDL2::SDL2 
SDL2_ttf::SDL2_ttf
SDL2_mixer::SDL2_mixer
SDL2_image::SDL2_image
unofficial-sodium::sodium)

# Make an executable that runs all the tests the provided files
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

# Copy test files in the output folder of the AllTests target
add_custom_command(
  TARGET AllTests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/testdata/"
          "$<TARGET_FILE_DIR:AllTests>"
)

# Note: We set the working dir of the test to the target output folder otherwise is runs tests from builddir and can't find the configuration files as they are not in the build dir, but the target dir (eg. Release/ etc)
add_test(NAME MainTests COMMAND AllTests WORKING_DIRECTORY $<TARGET_FILE_DIR:AllTests>)

include(GNUInstallDirs)

install(TARGETS cppgamelib 
EXPORT cppgamelib # produce target exports for this target
FILE_SET public_headers
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Create/write the targets exports to the specified file name in the lib directory (as this is where downstream packages look for it via find_package())

install(EXPORT cppgamelib
DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppgamelib
NAMESPACE cppgamelib::
)

# Write a basic package config version file for us please
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cppgamelib/cppgamelibConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# set a variable for with location of where we are going to install the config package to

set(ConfigPackageLocation lib/cmake/cppgamelib)

# install the package config file (this imports the exported target file previously created) and the version file
install(
  FILES
    cmake/cppgamelibConfig.cmake # This file must be created by us
    "${CMAKE_CURRENT_BINARY_DIR}/cppgamelib/cppgamelibConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
