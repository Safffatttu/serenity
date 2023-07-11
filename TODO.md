Profiling
DONE:
- Use nanoseconds for profiling
- "filesystem_type" in json file
- fixed /dev paths

TODO:

- Click system server(8) then /dev
- Optimize FileEventNode::find_or_create_node

// FixMe: This is a hack to make the compiler pack this struct correctly.

- Packed stuff - PackedErrorOr

Qprofiler
- stack trace walking
- Performance of profiling itself

pick 6fae634f6b Kernel+ProfileViewer: Display additional filesystem events
pick 439f35d970 Audio local
pick ea16428c34 Kernel: Profiling strings are now protected by spinlock
pick c75eb924fd Kernel+ProfilerViewer: Filesystem event duration is has now nanosecond resolution
pick 3569012276 Kernel+ProfilerViewer: Changed misleading field name
pick 53496a350d ProfileViewer: Fixed creating a tree for paths that had only one part
