# ------------------------------------------------------------------------------
# Copyright [2023] [Kadir PEKEL]
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# 	http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------------------

PROJECT = tron

CC = gcc
CPPFLAGS = -Wall -g
CFLAGS = `llvm-config --cflags` -fsanitize=address -fno-omit-frame-pointer -g -O1
LDFLAGS = `llvm-config --ldflags` -fsanitize=address 
LIBS = `llvm-config --libs`
SRC_DIR = src
OBJ_DIR = obj
FIXTURE = fixture

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

$(OBJ_DIR)/$(PROJECT): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(OBJ_DIR)/$(PROJECT) $(LIBS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

.PHONY : clean $(PROJECT)

fixture: $(OBJ_DIR)/corelib.o $(OBJ_DIR)/$(PROJECT)
	$(OBJ_DIR)/$(PROJECT) example/$(FIXTURE).tr $(OBJ_DIR)/$(FIXTURE).o
	$(CC) -fsanitize=address $(OBJ_DIR)/$(FIXTURE).o $(OBJ_DIR)/corelib.o -o $(OBJ_DIR)/$(FIXTURE)
	ASAN_OPTIONS=verbosity=0 $(OBJ_DIR)/$(FIXTURE)

clean:
	@rm -rf $(OBJ_DIR)
