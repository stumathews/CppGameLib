#include <atomic>
#include <memory>

namespace gamelib
{
    template <typename T>
    class DoubleBuffer 
    {
    public:
        DoubleBuffer() 
        {
			// Front buffer is for the consumer to read, back buffer is for the producer to write
            front = std::make_shared<T>();

			// Back buffer is for the producer to write into, and will be swapped with the front buffer when published
            back = std::make_shared<T>();
        }

        // Consumer reads from the front buffer
        std::shared_ptr<T> GetReadBuffer() const
        {
            return front;
        }

        // Producer writes into the back buffer
        T& GetWriteBuffer() 
        {
            return *back;
        }

        // Producer publishes the back buffer
        void Publish() 
        {
            // Atomically swap front/back pointers
            front.swap(back);
        }


    private:
        std::shared_ptr<T> front;   // consumer reads
        std::shared_ptr<T> back;    // producer writes
    };
}
