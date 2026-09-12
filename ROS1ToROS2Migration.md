# ROS 2 Paradigm Shift: Systems Engineering & Architectural Friction Report

## Overview
ROS 1 was designed as a rapid prototyping framework for single-robot academic research. ROS 2 was engineered as a ground-up rewrite targeting enterprise production, safety-critical systems, and multi-robot fleets over unreliable networks. To achieve this, ROS 2 strips away background automation ("magic") and replaces it with deterministic, explicit configurations. This increases production stability but directly increases development friction, boilerplate code, and architectural complexity.

---

## 1. Node Topology & Parameter Isolation

### The `declare_parameter()` Mandate
* **ROS 1 Paradigm:** Parameters lived globally on a central parameter server (`roscore`). Any node could query any parameter at runtime dynamically via `ros::param::get()`.
* **ROS 2 Paradigm:** Parameters are strictly localized to the node that owns them. Furthermore, a node **cannot read a parameter unless it explicitly declares it first** in its source code using `this->declare_parameter<T>()`.
* **Engineering Friction:** If an external XML launch file or a user-facing YAML file tries to pass a configuration parameter that your source code has not explicitly initialized, the node will silently ignore it or throw a runtime crash. This forces extensive constructor boilerplate just to accept standard configurations.

### The Loss of Global Parameter Sharing
* **ROS 1 Paradigm:** Node A could seamlessly read or modify a parameter belonging to Node B directly from the global server.
* **ROS 2 Paradigm:** Because parameters are local to a node's memory footprint, external nodes cannot access them natively.
* **Engineering Friction:** If Node A needs a configuration value from Node B, you can no longer run a simple query. You must implement an asynchronous ROS 2 Service client, request the parameters from Node B over the network, spin a background executor thread, and handle the asynchronous response callback.

---

## 2. Threading, Execution, & Concurrency Models

### The Hidden Callback Deadlock Trap
* **ROS 1 Paradigm:** Node callbacks (subscribers, timers, services) were managed by an automated background thread pool via `ros::spin()`. Concurrency was largely handled out-of-sight.
* **ROS 2 Paradigm:** Nodes do not process incoming network data on their own. They must be explicitly assigned to an **Executor** object (`SingleThreadedExecutor` or `MultiThreadedExecutor`).
* **Engineering Friction:** The default layout is a `SingleThreadedExecutor`. If a callback inside your node blocks to wait for a ROS Service response, a parameter update, or a hardware timeout, **the node will instantly deadlock itself**. It is trapped waiting for a network event, but because it only possesses one thread, it cannot process the incoming response message.

### Callback Groups Boilerplate
* **Architectural Fix:** To prevent execution deadlocks, developers cannot write simple, linear code blocks. You are forced to manually architect **Callback Groups** (`MutuallyExclusive` or `Reentrant`), explicitly assign specific groups to every individual subscriber, timer, and service client in your C++/Python constructor, and pass the node into a `MultiThreadedExecutor`.

---

## 3. Orchestration & Testing Constraints

### Launch Frontend Fragmentation
* **ROS 1 Paradigm:** A single XML standard (`.launch`) handled namespacing, conditional logic (`if`/`unless`), and arguments cleanly and concisely.
* **ROS 2 Paradigm:** The launch infrastructure was rewritten entirely in Python (`.launch.py`). While XML and YAML frontends were bolted back on later, they are stripped of key functionalities.
* **Engineering Friction:** The XML `<include>` tag does not natively support a `namespace` or `ns` attribute. Forcing an included production file into an isolated test sandbox requires wrapping it in a specialized, verbose `<group scoped="true" push-ros-namespace="test_sandbox">` block.

### Automated Testing Constraints (`launch_test.py`)
* **ROS 1 Paradigm:** An integration test was achieved by appending a simple 5-line `<test>` tag inside a standard launch XML file using `rostest`.
* **ROS 2 Paradigm:** The automated verification harness (`colcon test`) hooks into a backend python script that strictly expects an object-oriented Python class layout containing specific lifecycle hooks (`generate_test_description()`).
* **Engineering Friction:** Passing a pure XML file directly into the standard `add_launch_test()` CMake macro causes the backend python test loader to crash with an unhandled `NoneType` error. Automation requires writing an otherwise redundant Python pass-through script simply to satisfy the testing engine's framework requirements.

---

## 4. Build Systems & OS Interactions

### Standard Output (stdout) Block Buffering
* **ROS 1 Paradigm:** Standard console prints (`std::cout`, Python `print()`) automatically flushed line-by-line to the screen immediately.
* **ROS 2 Paradigm:** When a launch file spawns nodes as background subprocesses, the host operating system treats stdout as a block-pipe.
* **Engineering Friction:** Unless you use native ROS logging macros or explicitly append the terminal emulation attribute `emulate_tty="true"` to your launch node configurations, your logs will get caught in an OS cache pipe and only flush to the screen every few thousand characters, making live debugging impossible.

### Workspace Isolation & Symlinks
* **ROS 1 Paradigm:** `catkin_make` compiled everything into a unified target space. Changes to scripts or configurations were instantly active.
* **ROS 2 Paradigm:** `colcon` uses strict package isolation.
* **Engineering Friction:** If you modify a launch file or a YAML parameter file, they do not execute from your source directory. You must explicitly re-run `colcon build`, or remember to permanently append the `--symlink-install` flag to your compilation steps to link your source files directly to the installation layout.

---

## 5. Network Discovery & DDS Profiles

### Shared Network Broadcast Storms
* **ROS 1 Paradigm:** A central master (`roscore`) kept network traffic localized. Nodes only talked to each other if explicitly directed to the master IP.
* **ROS 2 Paradigm:** The system uses decentralized **DDS (Data Distribution Service)**, which discovers other nodes by blasting UDP multicast discovery packets across the entire network interface.
* **Engineering Friction:** If you boot a ROS 2 robot on a shared office or university Wi-Fi network without modifying your settings, it will automatically discover, link to, and attempt to process data from **every single other person's ROS 2 nodes** on that network, causing cross-talk and massive bandwidth flooding. Isolation requires calculating and enforcing strict `ROS_DOMAIN_ID` environment variables.

### Silent Quality of Service (QoS) Mismatches
* **ROS 1 Paradigm:** Sockets connected natively using standard, reliable network configurations.
* **ROS 2 Paradigm:** DDS introduces individual Quality of Service profiles (Reliability, Durability, History, Liveliness).
* **Engineering Friction:** If Node A sets its publisher reliability to "Best Effort" (common for sensors like LiDAR) and Node B sets its subscriber expectation to "Reliable", **they will silently refuse to connect**. No error is thrown, no warning is logged, and the topic will appear completely empty, creating difficult debugging scenarios.

### Security Operational Overhead (SROS2)
* **ROS 1 Paradigm:** No native security infrastructure existed.
* **ROS 2 Paradigm:** Includes a fully integrated security framework (SROS2) built into the DDS layer.
* **Engineering Friction:** Enabling security means you cannot simply execute a node. You must build, distribute, and maintain an active **Public Key Infrastructure (PKI)** certificate tree across every machine, complete with distinct identity certificates and strict XML governance access policies for every individual topic and service point.
