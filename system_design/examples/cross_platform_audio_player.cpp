// cross platform audio player
// example of abstract factory

// note: each OS has it's own way of creating audio decoders, and audio players

// design ideas: use abstract factory
// 1. Define the abstract factory: AudioSystemFactory
// That factory must declare factory methods for creating audio decoders, and audio players
// 2. Define the concrete factories
// There should be a factory for WindowsAudio, and one for LinuxAudio. Each must define products
// These concrete define concrete methods for creating audio decoders/players
// 3. Define abstract products: An audio decoder, and audio player
// 4. Define concrete products: Each of these for windows/linux

#include <iostream>
#include <memory>

// create the abstract products
class AudioDecoder
{
public:
    virtual void decode() const = 0;
    virtual ~AudioDecoder() = default;
};

class AudioPlayer
{
public:
    virtual void play() const = 0;
    virtual ~AudioPlayer() = default;
};

// create concrete products
class WindowsAudioDecoder: public AudioDecoder
{
public:
    void decode() const override
    {
        std::cout << "Using windows audio decoder" << std::endl;
    }
};

class LinuxAudioDecoder: public AudioDecoder
{
public:
    void decode() const override
    {
        std::cout << "Using linux audio decoder" << std::endl;
    }
};

class WindowsAudioPlayer: public AudioPlayer
{
public:
    void play() const override
    {
        std::cout << "Using windows audio player" << std::endl;
    }
};

class LinuxAudioPlayer: public AudioPlayer
{
public:
    void play() const override
    {
        std::cout << "Using linux audio player" << std::endl;
    }
};

// create the abstract factory
class AudioSystemFactory
{
public:
    virtual std::unique_ptr<AudioDecoder> createDecoder() const = 0;
    virtual std::unique_ptr<AudioPlayer> createPlayer() const = 0;
    virtual ~AudioSystemFactory() = default;
};

// create specific factories
class WindowsAudioSystemFactory: public AudioSystemFactory
{
public:
    std::unique_ptr<AudioDecoder> createDecoder() const override
    {
        return std::make_unique<WindowsAudioDecoder>();
    }

    std::unique_ptr<AudioPlayer> createPlayer() const override
    {
        return std::make_unique<WindowsAudioPlayer>();
    }
};

class LinuxAudioSystemFactory: public AudioSystemFactory
{
public:
    std::unique_ptr<AudioDecoder> createDecoder() const override
    {
        return std::make_unique<LinuxAudioDecoder>();
    }

    std::unique_ptr<AudioPlayer> createPlayer() const override
    {
        return std::make_unique<LinuxAudioPlayer>();
    }
};

class AudioClient
{
public:
    AudioClient(const AudioSystemFactory& factory): audio_system_factory_(factory) {}

    void run()
    {
        // now the client code can run on any type... be it windows or mac :)
        auto decoder = audio_system_factory_.createDecoder();
        auto player = audio_system_factory_.createPlayer();

        decoder->decode();
        player->play();
    }

private:
    const AudioSystemFactory& audio_system_factory_;
};

int main()
{
    // use factories to create objects
    // don't use heap memory/dynamic memory allocation if you don't have to
    // here, the stack objects are polymorphic by reference
    const auto windows_factory = WindowsAudioSystemFactory();
    auto windows_client = AudioClient(windows_factory);
    windows_client.run();

    const auto linux_factory = LinuxAudioSystemFactory();
    auto linux_client = AudioClient(linux_factory);
    linux_client.run();
}