# 1.Super::BeginPlay();

> ✅ **调用父类（基类）中实现的 `BeginPlay()` 函数，确保父类的初始化逻辑也被执行。**

---

### 📘 背景

在 Unreal Engine 中，`BeginPlay()` 是一个虚函数，表示游戏开始（或 Actor 被激活）时调用的初始化方法。

当你在子类中重写了 `BeginPlay()`，如果**不手动调用 `Super::BeginPlay()`**，那么：

* 父类的 `BeginPlay()` 中的逻辑就不会执行；
* 这可能导致例如组件未激活、输入未注册、默认行为丢失等问题。

# 2.纯函数
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
# 3.UCLASS(Abstract) 
将 Abstract 添加到 UCLASS 宏中用于标记该类为 抽象基类，禁止直接实例化;

>核心作用:
1.禁止创建实例：标记为 Abstract 的类无法在 UE 编辑器中被直接拖放到场景或蓝图继承，只能作为其他类的父类使用。
2.强制派生：确保开发者必须通过子类实现具体功能，符合面向对象设计中的抽象类原则。

# 4.为什么使用 TObjectPtr 代替传统的裸指针（如 USkeletalMeshComponent*）来引用骨架网格体（Skeletal Mesh）或其他 UObject 派生类对象

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

# 5.FName 和 FString 的区别：

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

# 6.✅ 为什么前向声明也能用？
因为你写的是：
```cpp
TObjectPtr<UInputMappingContext> AuraContext;
```
这只需要知道 `UInputMappingContext` 是一个类即可（类型大小是已知的智能指针，不需要完整类型）。

---

#### 🚨 但是注意：

这种写法**只在以下情况下有效**：

| 情况                                                                | 能否只写前向声明？                    | 原因           |
| ----------------------------------------------------------------- | ---------------------------- | ------------ |
| 作为指针 `UInputMappingContext*`、或 `TObjectPtr<UInputMappingContext>` | ✅ 可以                         | 不需要知道类型大小    |
| 作为值类型（直接构造对象）`UInputMappingContext obj;`                          | ❌ 不行                         | 编译器必须知道类完整定义 |
| 使用类成员函数或访问字段 `AuraContext->SomeFunction()`                        | ❌ 不行（在 .cpp 中必须 include 完整头） |              |

# 7.bReplicated
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250506105559866.png)

#### 🔍 具体解释：

#### `bReplicates` 是 `AActor` 的一个布尔成员变量：

* 设置为 `true`，表示 **这个 Actor 会自动在服务器和客户端之间进行同步**。即**网络同步（Replication）**
* 默认情况下，某些 Actor（如 PlayerController）可能不会自动启用。
* 在多人游戏中，**你必须设置 `bReplicates = true` 才能让属性/函数通过网络传播**。
* 常用于：`Pawn`、`Character`、`PlayerController`、`GameState`、自定义 `Actor` 等。

# 8.class和struct

## ✅ 1. **UE 的命名规则：`F` 开头的是 `struct`，`U` 开头的是 `UCLASS`**

| 前缀  | 含义                   | 示例                                           |
| --- | -------------------- | -------------------------------------------- |
| `F` | 普通数据结构（`struct`）     | `FVector`, `FHitResult`, `FInputActionValue` |
| `U` | UObject 派生类（`class`） | `UInputAction`, `UActorComponent`            |
| `A` | Actor 类（`class`）     | `ACharacter`, `APlayerController`            |
| `I` | 接口类                  | `IAbilitySystemInterface`                    |
| `E` | 枚举类型                 | `EInputEvent`, `EPhysicalSurface`            |

所以：

* `FInputActionValue` 是一个简单的值封装类型 → 用 `struct`
* `UInputAction` 是一个资源资产对象（继承自 `UObject`）→ 用 `UCLASS`

---

## ✅ 2. UE4/UE5 中 `struct` 和 `class` 没本质差别（只是默认权限不同）

C++ 中：

* `class`：默认成员是 `private`
* `struct`：默认成员是 `public`

Unreal 为了表达“轻量数据类型”，会倾向使用 `struct`。

# 9. 回调函数
这行代码是 Unreal Engine 5 中使用 **Enhanced Input 系统** 绑定输入动作的标准方式：

```cpp
EnhancedInputComponent->BindAction(
    MoveAction,
    ETriggerEvent::Triggered,
    this,
    &AAuraPlayerController::Move
);
```

---

## ✅ 简明解释：

> 当 `MoveAction` 被触发（Triggered）时，调用当前对象 `this` 上的 `Move` 函数。

---

### 参数含义：

| 参数                             | 含义                                      |
| ------------------------------ | --------------------------------------- |
| `MoveAction`                   | 要监听的输入动作（`UInputAction*` 类型）            |
| `ETriggerEvent::Triggered`     | 触发时机（例如按钮被按下时）                          |
| `this`                         | 绑定到哪个对象（这里是当前的 `AAuraPlayerController`） |
| `&AAuraPlayerController::Move` | 要调用的回调**函数指针**（成员函数）                        |

---

## 🔁 回调函数是什么？

> 回调函数（Callback）是指在特定事件发生时，**被自动调用的函数**。

在这里，回调函数就是：

```cpp
void AAuraPlayerController::Move(const FInputActionValue& InputActionValue);
```

它会在玩家执行了 `MoveAction`（例如：按下左摇杆、WASD 键）后被触发。

# 10. Rotator
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250508144128804.png)
## Rotator(Pitch,Yaw,Roll)->(绕Y,绕Z,绕X)
## 获取角色朝向(常用)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250508153911215.png)
# 11.安全检查


| 写法                       | 作用                       |
| ------------------------ | ------------------------ |
| `check(ControlledPawn);` | 如果是空指针，会**崩溃报错（断言失败）**   |
| `if (ControlledPawn)`    | 如果是空指针，**什么也不做**，安全退出  ✅ |

### ✅ 为什么不用 `check()`？

* 游戏运行时可能出现 `GetPawn()` 为空（比如刚开始没有控制 Pawn）
* 如果你断言它一定不为空，**一旦为空就直接崩溃**
* `if` 是更**稳健**、更**容错**的写法

---

#  12. `GetPawn()`和 `GetPawn<APawn>()`？

```cpp
APawn* ControlledPawn = GetPawn();        // ✅ 正确
APawn* ControlledPawn = GetPawn<APawn>(); // ✅ 也是正确（UE5新写法）
```

这两种写法都是可以的，区别如下：

| 写法                 | 是否推荐                   | 说明 |
| ------------------ | ---------------------- | -- |
| `GetPawn()`        | ✅ 常规写法，返回 `APawn*`     |    |
| `GetPawn<APawn>()` | ✅ 更现代、模板安全，**多用于子类转换** |    |

### 🧠 用途区别：

* `GetPawn()`：返回当前控制的 Pawn（你自己知道它类型是 `APawn`）
* `GetPawn<AMyCustomCharacter>()`：**可以直接返回你自定义的子类指针，省去 `Cast`**

# 13. Run & Debug


| 功能     | Run ▶️      | Debug 🐞            |
| ------ | ----------- | ------------------- |
| 启动方式   | 快速启动，不附加调试器 | 启动 + 附加调试器          |
| 是否能打断点 | ❌ 否（断点不生效）  | ✅ 是（断点、变量、堆栈都可用）    |
| 使用场景   | 日常运行、调试蓝图   | 调试 C++ Bug、逻辑、崩溃跟踪等 |
| 启动速度   | 较快          | 略慢（因附加调试器）          |

# 14. 虚函数 & 纯虚函数 & 抽象类 & 接口

---

## ✅ 1. **虚函数（virtual function）**

```cpp
class Base {
public:
    virtual void Foo() {
        cout << "Base Foo" << endl;
    }
};
```

### 含义：

* 虚函数支持**多态**（运行时绑定）
* 父类指针或引用调用时，**会执行子类的重写版本**

```cpp
class Child : public Base {
public:
    void Foo() override {
        cout << "Child Foo" << endl;
    }
};

Base* b = new Child();
b->Foo();  // 输出 "Child Foo"
```

---

## ✅ 2. **纯虚函数（pure virtual function）**

```cpp
class IAnimal {
public:
    virtual void Speak() = 0;  // 纯虚函数
};
```

### 含义：

* `= 0` 表示**没有默认实现**
* 子类**必须重写**这个函数
* 类中**有纯虚函数的类，叫做抽象类**

---

## ✅ 3. **抽象类（abstract class）**

```cpp
class IAnimal {
public:
    virtual void Speak() = 0;  // 没有实现
};

IAnimal a;  // ❌ 错误：抽象类不能实例化
```

### 特点：

* 不能创建对象（`IAnimal a;` ❌）
* 只能作为**接口或基类**使用

---

## ✅ 4. **接口（interface）**

在 C++ 中没有关键字 `interface`，但通过**抽象类 + 全部是纯虚函数**的写法模拟接口：

```cpp
class IHighlightable {
public:
    virtual void HighlightActor() = 0;  // 接口函数
};
```

任何类只要继承它并实现函数，就等于**实现了接口**。

---

## 🔁 对比总结：

| 名称     | 写法                        | 是否必须实现 | 是否能实例化 | 主要作用      |
| ------ | ------------------------- | ------ | ------ | --------- |
| 虚函数(有默认实现)    | `virtual void Foo()`      | ❌ 可选   | ✅ 可以   | 支持多态      |
| 纯虚函数(无默认实现)   | `virtual void Foo() = 0;` | ✅ 必须   | ❌ 不行   | 定义接口、强制实现 |
| 抽象类    | 含有纯虚函数的类                  | -      | ❌ 不行   | 接口或基类     |
| 接口（模拟） | 抽象类 + 全部纯虚函数              | ✅ 必须   | ❌ 不行   | 明确行为规范    |

