#pragma once
#include "Status.h"
#include "Composite.h"

namespace gamelib
{
	namespace ai
	{
		// A sequence composite runs each child behavior in order until one fails
		class Sequence : public Composite  // NOLINT(cppcoreguidelines-special-member-functions)
		{
		protected:
			~Sequence() override = default;

			virtual void OnInitialize() override
			{
				// Start running the first child in sequence
				currentChild = children.begin();
			}

			Status Update(const unsigned long deltaMs) override
			{
				// Update each child
				while (currentChild != children.end())
				{
					// Run the child behavior
					const Status childStatus = (*currentChild)->Tick();

					// Check what happened 
					if (childStatus != Status::Success)
					{
						// If any child fails, we fail too
						return childStatus;
					}

					// Move to the next child to run...
					++currentChild;
				}

				// All children were updated
				return Status::Success;
			}

			Behaviors::iterator currentChild;
		};
	}
}
