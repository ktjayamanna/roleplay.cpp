# Comprehensive architectural layout for adding binary file support to the server server framework

**Feature:** Add binary file upload/download support to HTTP server framework  

---

## 📋 Overview

We're extending our modular HTTP server to handle binary files (images, audio, PDFs, etc.) in addition to JSON responses. This is a **progressive implementation** - each phase builds on the previous one.

### Why This Matters
- Current server only handles text/JSON responses
- Binary data contains null bytes (`\0`) which break string functions like `strlen()` and `strcpy()`
- Need proper Content-Type headers for browsers to display files correctly
- Must track data length explicitly, not rely on null-termination

---

## 🎯 Phase 1: Binary Response Support (START HERE)

**Goal:** Enable endpoints to return binary files to clients

### Step 1.1: Update `EndpointResponse` Structure
**File:** `server/server.h` (lines 35-50)

**Task:** Uncomment and add the TODO fields:
```c
typedef struct EndpointResponse {
    int status_code;
    char* body;
    char* content_type;
    size_t body_length;    // ADD THIS
    int is_binary;          // ADD THIS
} EndpointResponse;
```

**Why:**
- `body_length`: Binary data can't use `strlen()` (stops at first null byte)
- `is_binary`: Tells HTTP layer how to handle the data

---

### Step 1.2: Update `endpoint_create_response()`
**File:** `server/endpoint.c` (lines 135-150)

**Task:** Initialize the new fields in the existing function:
```c
response->body_length = body ? strlen(body) : 0;
response->is_binary = 0;  // Text response by default
```

**Why:** Backward compatibility - existing text endpoints still work

---

### Step 1.3: Implement `endpoint_binary_response()`
**File:** `server/endpoint.c` (lines 180-210)

**Task:** Implement the function following the TODO comments

**Key Points:**
- Use `malloc()` to allocate `body_length` bytes
- Use `memcpy()` NOT `strcpy()` (binary data has null bytes!)
- Set `is_binary = 1`
- Set `body_length` to actual data size

**Common Mistakes to Avoid:**
- ❌ `strcpy(response->body, data)` - WRONG! Stops at first null byte
- ✅ `memcpy(response->body, data, data_length)` - CORRECT!

---

### Step 1.4: Implement `endpoint_file_response()`
**File:** `server/endpoint.c` (lines 212-247)

**Task:** Read file from disk and create binary response

**Algorithm:**
1. Open file with `fopen(file_path, "rb")` - **"rb" = read binary mode**
2. Get file size: `fseek(file, 0, SEEK_END)` then `ftell(file)`
3. Rewind: `fseek(file, 0, SEEK_SET)`
4. Allocate buffer: `malloc(file_size)`
5. Read: `fread(file_data, 1, file_size, file)`
6. Detect content type from extension (see TODO comments)
7. Call `endpoint_binary_response()` with the data
8. Clean up: `free(file_data)` and `fclose(file)`

**Content Type Mapping:**
- `.mp3` → `"audio/mpeg"`
- `.png` → `"image/png"`
- `.jpg` → `"image/jpeg"`
- `.pdf` → `"application/pdf"`
- Unknown → `"application/octet-stream"`

---

### Step 1.5: Declare Functions in Header
**File:** `server/endpoint.h` (lines 46-80)

**Task:** Uncomment the function declarations

---

### Step 1.6: Implement `http_build_binary_response()`
**File:** `server/http.c` (lines 66-106)

**Task:** Build HTTP response with binary body

**Critical Detail:**
```c
// Build headers first
int header_length = sprintf(buffer, "HTTP/1.1 200 OK\r\n...");

// Then append binary body using memcpy
memcpy(buffer + header_length, body, body_length);
```

**Why memcpy?** Binary data may contain `\0` bytes. String functions stop there!

---

### Step 1.7: Declare Binary Response Function
**File:** `server/http.h` (lines 41-59)

**Task:** Uncomment the function declaration

---

### Step 1.8: Update `handle_route()` to Use Binary Responses
**File:** `server/server.c` (lines 117-163)

**Task:** Replace the existing `http_build_response()` call with conditional logic:

```c
HttpResponse* http_response;
if (endpoint_response->is_binary) {
    http_response = http_build_binary_response(
        endpoint_response->status_code,
        endpoint_response->body,
        endpoint_response->body_length,
        endpoint_response->content_type
    );
} else {
    http_response = http_build_response(
        endpoint_response->status_code, 
        endpoint_response->body
    );
}
```

---

### Step 1.9: Fix Binary Transmission in `handle_client()`
**File:** `server/server.c` (lines 165-201)

**Problem:** Current code uses `strlen(response)` which breaks for binary data

**Solution:** Change `handle_route()` to return `HttpResponse*` instead of `char*`

**Refactor:**
1. Change signature: `HttpResponse* handle_route(char* method, char* url)`
2. Return `http_response` directly (don't convert to string)
3. In `handle_client()`:
   ```c
   HttpResponse* response = handle_route(method, url);
   write(client_fd, response->body, response->body_length);
   free(response->body);
   free(response);
   ```

**Why:** `HttpResponse` already has `body_length` field - use it!

---

## 🧪 Testing Phase 1

### Test 1: Create a Binary File Endpoint
**File:** `examples/music_server.c` (create new file)

```c
#include "../server/server.h"
#include "../server/endpoint.h"

EndpointResponse* serve_music(const RequestContext* request) {
    return endpoint_file_response(200, "examples/music.mp3");
}

int main() {
    server_init(8080);
    server_register_handler("/music.mp3", "GET", serve_music);
    printf("Music server running on http://localhost:8080/music.mp3\n");
    server_start();
    return 0;
}
```

### Test 2: Build and Run
```bash
cd examples
make APP_NAME=music_server
./build/music_server.exe
```

### Test 3: Verify in Browser
Open: `http://localhost:8080/music.mp3`

**Expected:** Browser plays or downloads the MP3 file

**Debug Checklist:**
- [ ] File opens successfully (check `fopen()` return value)
- [ ] File size is correct (print `file_size` before reading)
- [ ] `is_binary` flag is set to 1
- [ ] Content-Type header is `"audio/mpeg"`
- [ ] Response body_length matches file size

---

## 📊 Phase 2: Request Content-Type Tracking (FUTURE)

**Goal:** Handle binary uploads from clients

### Step 2.1: Add Content-Type to RequestContext
**File:** `server/server.h` (lines 25-42)

Uncomment the TODO field:
```c
char content_type[128];
```

### Step 2.2: Parse Content-Type Header
**File:** `server/http.c`

Add function to extract Content-Type from HTTP request headers

### Step 2.3: Update `endpoint_dispatch()`
**File:** `server/endpoint.c`

Pass content_type to RequestContext

---

## 🎓 Learning Resources

### Key C Concepts
1. **Binary vs Text Mode:**
   - `fopen("file.txt", "r")` - text mode (converts line endings)
   - `fopen("file.bin", "rb")` - binary mode (raw bytes)

2. **String vs Memory Functions:**
   - `strlen()` - stops at `\0`
   - `strcpy()` - stops at `\0`
   - `memcpy()` - copies exact number of bytes (use for binary!)

3. **File I/O:**
   - `fseek(file, 0, SEEK_END)` - go to end
   - `ftell(file)` - get current position (= file size)
   - `fread(buffer, 1, size, file)` - read `size` bytes

### Common Pitfalls
1. **Using strlen() on binary data** → Returns wrong length
2. **Using strcpy() for binary data** → Truncates at null bytes
3. **Forgetting to set is_binary flag** → Server treats binary as text
4. **Wrong fopen() mode** → Use "rb" not "r" for binary files

---

## ✅ Completion Checklist

### Phase 1 Complete When:
- [ ] All TODO comments in Phase 1 are implemented
- [ ] Code compiles without errors
- [ ] `music_server` example serves MP3 file correctly
- [ ] Browser can play/download the file
- [ ] No memory leaks (run with `valgrind` if available)

### Code Review Points:
- [ ] All binary data uses `memcpy()` not `strcpy()`
- [ ] All file operations check return values
- [ ] Memory is properly freed (no leaks)
- [ ] `is_binary` flag is set correctly
- [ ] Content-Type headers match file types

---

## 🚀 Next Steps After Phase 1

1. **Add more file types** (PNG, PDF, etc.)
2. **Implement file upload** (Phase 2)
3. **Add streaming for large files** (chunked transfer)
4. **Add caching headers** (ETag, Last-Modified)

---

## 💡 Tips for Success

1. **Compile frequently** - Don't write everything then compile
2. **Test incrementally** - Test each function as you write it
3. **Print debug info** - Use `printf()` to verify values
4. **Read error messages** - Compiler errors tell you exactly what's wrong
5. **Ask questions** - If stuck for >30 minutes, ask for help!

---

**Good luck! Remember: Binary data is just bytes - treat it with memcpy(), not string functions! 🎯**

