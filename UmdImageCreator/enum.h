/**
 * Copyright 2018 sarami
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

typedef enum _LOG_TYPE {
	standardOut = 1,
	standardError = 1 << 1,
	fileDisc = 1 << 2,
	fileVolDesc = 1 << 3,
	fileDrive = 1 << 4,
	fileMainInfo = 1 << 5,
	fileMainError = 1 << 6,
	fileSubInfo = 1 << 7,
	fileSubIntention = 1 << 8,
	fileSubError = 1 << 9,
	fileC2Error = 1 << 10
} LOG_TYPE, *PLOG_TYPE;

typedef enum _PATH_TYPE {
	lType,
	mType
} PATH_TYPE, *PPATH_TYPE;
