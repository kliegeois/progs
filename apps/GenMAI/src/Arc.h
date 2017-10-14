//   Copyright 2017 Romain Boman
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#ifndef ARC_H
#define ARC_H

#include "Curve.h"

/**
 * @brief Defines an Arc of circle with 3 points.
 */

class Arc : public Curve
{
public:
    Arc(int p1, int p2, int p3);

    void print() const;
    char *name() const;
    char *carteBacon() const;
    int typeDon() const;
};

#endif
