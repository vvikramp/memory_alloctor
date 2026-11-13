- No extension is normal on macOS and Linux. Unlike .exe on Windows, a file is runnable because it has the "executable" permission, not because of its name.

- You can skip address casting completely by aligning the offset. This works because malloc already returns a 16-aligned start.

- _Align for structs and why "align a type by its size" may not be always right.