# 1.学习笔记
## 纯函数
---
在 **Unreal Engine 5 (UE5)** 中，**纯函数（Pure Function）** 是一个重要的概念，尤其在 **蓝图（Blueprint）** 和 **C++** 开发中。它指的是 **不修改游戏状态、没有副作用，且输出仅依赖于输入参数** 的函数。UE5 对纯函数有明确的标记和支持，主要用于优化性能和逻辑清晰性。


## **UE5 纯函数的优势**
### **1. 性能优化**
- **避免不必要的计算**  
  纯函数的结果可被缓存（Memoization），例如在蓝图中多次调用相同参数的纯函数，引擎可能优化为单次计算。
- **更安全的并行计算**  
  纯函数无共享状态，适合多线程环境（如 `AsyncTask`）。

### **2. 更清晰的蓝图逻辑**
- **减少 `Exec` 连线混乱**  
  纯函数直接嵌入数据流，使蓝图更易读。
- **适用于 `Getter` 和计算逻辑**  
  如 `GetPlayerHealth()`、`CalculateDamage()` 等。

### **3. 可预测性**
- **相同输入 → 相同输出**  
  适合游戏逻辑中的确定性计算（如物理模拟、AI 决策）。


## **纯函数 vs 非纯函数（UE5 示例）**
| 特性 | 纯函数 | 非纯函数 |
|------|--------|----------|
| **修改游戏状态** | ❌ 否 | ✅ 是 |
| **有 `Exec` 引脚** | ❌ 否 | ✅ 是 |
| **线程安全** | ✅ 是 | ❌ 否 |
| **适用场景** | 计算、查询 | 事件触发、状态修改 |


### **最佳实践**
- **尽量用 `const`**  
  如果函数不修改成员变量，标记为 `const`：
  ```cpp
  UFUNCTION(BlueprintPure)
  bool IsAlive() const; // 不修改对象状态
  ```
- **避免依赖全局状态**  
  如 `UGameplayStatics::GetPlayerController(0)` 可能因玩家索引变化导致非确定性。

# 2.UE5设置
1. 设置源码IDE
2. 禁止热重载，自动compile c++ class
3. 禁止live coding
4. 设置默认和起始地图
5. 确保rider集成插件打开
   
# 3.git 版本控制
1. 创建.gitignore文件，不会上传添加在列的文件

# 4.创建AuraCharacterBase基类
1.在ue5中下载用于调试的工具
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250412193313982.png)
2. 点击rider中的run按钮在非debug模式下运行，debug按钮在debug模式下运行
# 5.Player and Enemy Charecters
1. 将 Abstract 添加到 UCLASS 宏中用于标记该类为 抽象基类，禁止直接实例化;

>核心作用:
1.禁止创建实例：标记为 Abstract 的类无法在 UE 编辑器中被直接拖放到场景或蓝图继承，只能作为其他类的父类使用。
2.强制派生：确保开发者必须通过子类实现具体功能，符合面向对象设计中的抽象类原则。

2. 置为false，避免不必要的每帧更新，节省性能。
 ![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250412200058062.png)

3.更改文件目录结构，需要重新编译，删掉Binaries和intermedate文件夹
4. 添加派生类AuraCharactor,AuraEnemy


# 6.Character Blueprint Setup
## 为角色基类添加武器骨骼网格体
1. cpp文件中添加weapon子物体，并绑定socket
```cpp
Weapon=CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
Weapon->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
```
2. 创建角色蓝图，为角色添加SKM网格
3. 为角色SKM网格添加socket，并绑定preview
4. 回到蓝图中，为weapon添加SKM网格
# 为什么使用 TObjectPtr 代替传统的裸指针（如 USkeletalMeshComponent*）来引用骨架网格体（Skeletal Mesh）或其他 UObject 派生类对象

在 Unreal Engine 中，`TObjectPtr` 是一种智能指针类型，用于替代传统的裸指针（如 `USkeletalMeshComponent*`）来引用 `UObject` 派生类对象（如骨架网格体 `USkeletalMeshComponent`）。以下是使用 `TObjectPtr` 的优势和原因：

---

### 1. **类型安全与显式所有权**
   - **传统问题**：裸指针（`USkeletalMesh* Mesh`）无法明确区分“引用”和“所有权”，容易导致悬垂指针或内存泄漏。
   - **`TObjectPtr` 优势**：
     - 显式标识这是一个 `UObject` 类型的引用。
     - 与 UE 的垃圾回收（GC）系统深度集成，避免引用非法对象。
     ```cpp
     // 传统方式（潜在风险）
     USkeletalMesh* Mesh; 

     // 现代方式（推荐）
     TObjectPtr<USkeletalMesh> Mesh;
     ```

---

### 2. **自动处理垃圾回收（GC）**
   - **GC 兼容性**：`TObjectPtr` 会被 UE 的垃圾回收器跟踪，确保引用的对象不被意外回收。
   - **空引用保护**：当对象被 GC 销毁时，`TObjectPtr` 会自动置为 `nullptr`，避免野指针。
   ```cpp
   TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

   // 安全访问（GC 不会回收 SkeletalMesh 引用的对象）
   if (SkeletalMesh) 
   {
       SkeletalMesh->SetVisibility(true);
   }
   ```

---

### 3. **序列化与蓝图兼容**
   - **序列化支持**：`TObjectPtr` 能正确序列化到磁盘（如保存到 `.uasset`），而裸指针需要额外处理。
   - **蓝图友好**：`TObjectPtr` 属性可以在蓝图中安全暴露，编辑器能正确识别引用关系。

---

### 4. **性能优化（延迟加载）**
   - **按需加载**：结合 `LAZY_LOAD_PTR` 宏，`TObjectPtr` 支持延迟加载资源，减少内存占用。
   ```cpp
   // 定义延迟加载的骨架网格体
   LAZY_LOAD_PTR(USkeletalMesh, MyMesh, "/Game/Characters/Hero/SK_Hero");

   // 使用时自动加载（线程安全）
   if (MyMesh.IsPending())
   {
       MyMesh.LoadSynchronous();
   }
   ```

# FName 和 FString 的区别：

1. **FName**：

   * **优化**：`FName` 是一个轻量级的字符串类型，特别用于高效比较和查找。它通过将字符串映射到一个全局唯一的名字池中，避免了重复字符串的存储，从而减少内存使用。
   * **不可变性**：`FName` 是不可变的，也就是说，一旦创建后，其值就不能改变。
   * **使用场景**：适用于频繁比较的情况，例如在游戏中使用标签、ID、属性名等场景。

2. **FString**：

   * **功能更强**：`FString` 是一个动态字符串类，支持丰富的字符串操作，如修改、拼接等。它在内存中存储完整的字符数据，适用于需要频繁修改或操作字符串的场景。
   * **内存开销较大**：`FString` 会存储整个字符串内容，可能会占用更多内存，特别是在字符串长度较长或频繁创建时。
   * **使用场景**：适用于需要经常操作和修改字符串内容的场合，例如显示文本、读取和写入文件时。

### 总结：

* `FName` 更适合用于需要高效比较和查找的情况（例如ID或标签）。
* `FString` 更适合需要进行大量字符串操作和修改的情况（例如文本显示和处理）。


# 7.Animation Blueprint
# 为主角添加动画蓝图
## 1. 创建ABP_Aura
## 2. 创建state machine “Main States”![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250504143131869.png)
  ### Slot
>- Slot 节点用于在动画蓝图中为动画蒙太奇（Montage）预留一个“插槽”位置，插入覆盖动画。
>- 如果播放某个 Montage 动画（如攻击、跳跃），它会插入到这个 Slot 中，并 覆盖 状态机的当前动画（比如正在跑步时切换成攻击动画）。
## 3. 添加“IdleWalkRun” State
## 4. 重写 InitialAnimation获取characterMovement ![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250504145346552.png)
这张图展示的是 **Unreal Engine 动画蓝图（AnimBlueprint）中 `Event Blueprint Initialize Animation` 节点的初始化逻辑**，它的主要作用是 **获取当前控制该动画的角色（Pawn/Character）引用** 并保存下来，以便后续动画更新时访问角色属性（如移动速度、状态等）。

---

### 🔍 各节点含义解释如下：

1. **`Event Blueprint Initialize Animation`**

   * 当动画蓝图第一次初始化时触发（比如角色刚生成、刚加载时）。
   * 适合做一些一次性的引用绑定（如保存角色对象、组件等）。

2. **`Try Get Pawn Owner`**

   * 获取这个动画蓝图当前附着的 Pawn（通常是角色角色）的引用。
   * 输出是 `APawn*` 类型（返回控制这个动画的角色对象）。

3. **`Cast to BP_AuraCharacter`**

   * 尝试将 `Pawn` 强制转换（Cast）为你自己创建的角色类（`BP_AuraCharacter`）。
   * 成功后可以访问 Aura 角色的特有属性和函数。
   * `Cast Failed` 用于处理失败情况（此处未连线，代表默认忽略失败）。

4. **`SET BP Aura Character`**

   * 把转换成功的 `BP_AuraCharacter` 引用存入动画蓝图的一个变量（比如你创建的 `BP_AuraCharacter` 类型变量），供其他地方调用。

5. **`Character Movement`**

   * 从 Aura Character 中提取 `CharacterMovement` 组件（控制角色移动逻辑的组件），以便动画蓝图后续使用移动状态（如速度、是否在地面等）。

6. **`SET Character Movement`**

   * 将提取的 `CharacterMovement` 组件保存为动画蓝图中的变量，供后续节点访问。

## 5. 根据角色的移动速度来设置动画蓝图中的 `Speed` 变量
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250504160643791.png)这张图展示的是 **Unreal Engine 动画蓝图中 `Event Blueprint Update Animation` 的逻辑**，它的主要作用是：**在每一帧更新时，根据角色的移动速度来设置动画蓝图中的 `Speed` 变量**，用于驱动角色的运动动画（如 Idle、Walk、Run 等）。

---

### 🔍 各节点含义说明：

#### 1. **`Event Blueprint Update Animation`**

* 每一帧都会被调用。
* 适用于实时获取角色状态（如速度、是否在地面等）以更新动画状态机所需变量。

---

#### 2. **`GET BP Aura Character` + `Is Valid`**

* 获取上面你初始化时保存的 `BP_AuraCharacter` 变量。
* 使用 `Is Valid` 节点进行有效性检查，防止引用为 null 时程序崩溃。
* 如果角色存在，则继续执行后面的逻辑。

---

#### 3. **`Character Movement → Velocity`**

* 使用之前保存的 `CharacterMovement` 组件，获取它的 **速度向量（`Velocity`）**。
* 这是一个 `Vector` 值，包含 X、Y、Z 方向的速度信息。

---

#### 4. **`Vector Length XY`**

* 计算 `Velocity` 向量的 **水平速度（X 和 Y 分量的长度）**，忽略垂直方向（Z），以排除跳跃或下落影响。
* 返回的是一个浮点数，表示单位/秒的移动速度。

---

#### 5. **`SET Speed`**

* 将刚刚计算出来的水平速度赋值给动画蓝图中的变量 `Speed`。
* 这个变量通常会用于动画状态机中，判断角色是否处于 Idle、Walk、Run 等状态。

---

### ✅ 总结作用：

每一帧中，这段逻辑会：

1. 确认角色引用有效；
2. 获取角色的当前移动速度；
3. 计算水平速度大小；
4. 设置 `Speed` 变量供动画系统使用。

---

## 6. 设置蓝图mesh的ABP

# 为敌人添加动画蓝图
## 1. 创建模板动画蓝图ABP_Enemy
## 2. 使用的是 **Blend Space（混合空间）** 来根据角色速度动态切换动画![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250504164228214.png)
这张图展示的是 **Unreal Engine 中动画状态机的一个状态（如 Running、Walking、Idle）内部的动画逻辑**，使用的是 **Blend Space（混合空间）** 来根据角色速度动态切换动画。

---

#### 2. **Blendspace Player**

* 这是一个 **Blend Space 动画节点**，可以根据输入的数值在多个动画之间平滑过渡。
* 它的 **X轴通常代表速度（Speed）**，用于控制在 Idle、Walk、Run 动画之间平滑切换。
* 此处 `Ground Speed` 被连到 X 输入，Y 为 0，说明这是一个 **一维（1D）Blend Space**（只根据速度混合）。

---
## 3. 创建子动画蓝图ABP_Goblin_Spear，设置混合空间动画
## 4. 设置蓝图mesh的ABP