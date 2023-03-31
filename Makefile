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
CFLAGS = 
LIBS = 
SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

$(PROJECT): $(OBJ_DIR) | $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJ) -o $(OBJ_DIR)/$(PROJECT) $(LIBS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

.PHONY : clean $(PROJECT)

clean:
	@rm -rf $(OBJ_DIR)
