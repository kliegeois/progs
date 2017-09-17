//   Copyright 1994 Igor Klapka
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

#ifndef VARARRAY_H
#define VARARRAY_H

extern int debug;

template <class T>
class VarArray
{
    T *data;
    int maxsz;
    int last;

  public:
    VarArray(int initsz = 8);
    VarArray(const VarArray<T> &);
    ~VarArray();

    void remove_one_record(int i);
    int maxsize();
    int size();
    T &operator[](int);
    VarArray<T> &operator=(const VarArray<T> &);

  private:
    void resize(int);
    void swap(int a, int b);
};

#include "VarArray.inl"

#endif // VARARRAY_H
