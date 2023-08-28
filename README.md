headerx - A header extractor for mixed header + c code files

While there are compelling reasons to keep headers and C code in separate files — especially when a single header encompasses definitions from multiple C sources — certain scenarios would benefit from more integrated approach. In some cases, the header is so closely tied to its C source file that it feels like an introductory segment of that source. For these situations, the header could be well placed directly within the corresponding C source file.

Why not take advantage of this closeness? We could embed the header within its C source file and then utilize build tools to extract it during compile time for external purposes. This way, the project retains a single, unified file that houses both the header and the C code. The advantages are clear: fewer files to manage and a seamless integration of declarations and definitions.

This approach is particularly apt when an object-oriented philosophy is applied to a C project. It's not uncommon in such projects to have a "one class per file" setup, or perhaps a small cluster of related classes within a single file. For these scenarios, a blended header and source approach could be beneficial.

While I recognize this approach might fire up a passionate debates, sparking such discussions isn't the intent behind this modest software piece. It's just experience!
