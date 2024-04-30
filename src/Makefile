# Copyright 2024 vivimice <vivimice@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

all: journalagent libjournalagent_interceptor.so

journalagent: journalagent.c
	gcc -O2 journalagent.c -o journalagent

libjournalagent_interceptor.so: libjournalagent_interceptor.c
	gcc -shared -O2 libjournalagent_interceptor.c -o libjournalagent_interceptor.so
