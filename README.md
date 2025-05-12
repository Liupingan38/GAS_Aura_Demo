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
#### 常用：
| 情况                            | 建议方式                    |
| ----------------------------- | ----------------------- |
| 用到类的成员变量/继承基类                 | `#include`              |
| 只用到指针/引用（函数参数、成员）             | 前向声明 + `.cpp` 中 include |
| 用到了模板类（如 `TArray<FMyStruct>`） | `.h` 中需要 `#include` 定义  |
| 蓝图类、控件类、组件类（如 `UUserWidget`）  | **尽量只 forward declare** |


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

# 15.`TScriptInterface<T>`

```cpp
TScriptInterface<IEnemyInterface> LastActor;
```

是 Unreal Engine 提供的用于**接口访问**的特殊包装类型，它的作用是：

> ✅ 允许你在 C++ 中**安全地存储并调用蓝图/C++接口的对象**

---

## ✅ 为什么不用普通指针（如 `IEnemyInterface*` 或 `AActor*`）？

因为 UE 的接口类有两个特点：

1. UE 接口类继承自 `UInterface`（不是普通 C++ 接口）
2. 一个类实现接口时，**需要同时继承 `UInterface` 和实现 `IYourInterface`**

直接用 `IEnemyInterface*` 访问接口函数在运行时容易出现失败或崩溃（因为 UObject 和接口地址不是同一个）。

---

## ✅ `TScriptInterface<T>` 是什么？

它是一个 **模板封装类型**，可以安全存储：

* 指向 `UObject` 的对象（比如 Actor）
* 并能自动获取并调用它的接口函数

---

## ✅ 它内部包含两个指针：

```cpp
struct TScriptInterface
{
    UObject* Object;         // 指向实现接口的 UObject
    void* InterfacePointer;  // 指向接口函数表
};
```

这样你就可以直接调用接口函数：

```cpp
if (LastActor) {
    LastActor->HighlightActor();  // 调用接口中的函数
}
```


你可以把任何实现了 `UEnemyInterface` 的 Actor（如敌人）赋值给 `LastActor`，并**安全调用接口方法**。

---

## ✅ 总结

| 写法                                  | 含义                              |
| ----------------------------------- | ------------------------------- |
| `TScriptInterface<IEnemyInterface>` | 存放实现接口的 Actor + 安全调用接口方法        |
| 不用 `IEnemyInterface*`               | 因为 UE 接口有双重继承，裸指针无法保证安全调用       |
| 蓝图兼容                                | 支持 BlueprintInterface，且能被蓝图赋值使用 |

非常好的问题！你可以这样赋值的原因在于：`TScriptInterface<IEnemyInterface>` 是 Unreal 引擎专为 **UInterface 接口系统设计的封装类**，它支持智能赋值和类型转换，下面我来**简明扼要**解释为什么这些赋值都成立：

---

## ✅ 你写的代码

```cpp
TScriptInterface<IEnemyInterface> LastActor;
TScriptInterface<IEnemyInterface> ThisActor;

LastActor = ThisActor;                          // ✅ 拷贝赋值，类型相同
ThisActor = CursorHit.GetActor();              // ✅ 隐式转换（前提：GetActor() 实现了接口）
```

---

## ✅ 为什么可以这样赋值？

### 🔹 1. `TScriptInterface<IEnemyInterface>` 本质上是结构体封装

```cpp
template<class InterfaceType>
class TScriptInterface
{
    UObject* Object;              // 指向 UObject 对象
    InterfaceType* Interface;    // 指向接口（通常是 IEnemyInterface*）
};
```

它内部自动处理了：

* 从 `AActor*` 获取接口指针（调用 `Cast<IEnemyInterface>()`）
* 判断该对象是否实现了接口

---

### 🔹 2. `ThisActor = CursorHit.GetActor();`

这是**从 `AActor*`（或 `UObject*`）赋值给接口类型**，能这样写的前提是：

* `CursorHit.GetActor()` 返回的是 `AActor*`
* 该 `AActor` 的类实现了 `UEnemyInterface`

UE 源码中重载了赋值操作符，使你能这样写：

```cpp
TScriptInterface<IEnemyInterface>::operator=(UObject* InObject)
```

它会自动：

* 设置内部的 `Object` 指针
* 尝试从 `InObject` 提取接口指针（运行时验证）

---

## ✅ 总结

| 表达式                                 | 是否允许 | 原因                  |
| ----------------------------------- | ---- | -------------------             |
| `ThisActor = CursorHit.GetActor();` | ✅ 是  | 自动从 `AActor*` 解析接口  |
| `TScriptInterface<T> = UObject*`    | ✅ 是  | 引擎重载了赋值运算符，自动解析接口指针 |

# 16.ECC/ECR
```cpp
GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 设置可视为阻挡
```

## ✅ 解释这行代码含义：

| 部分                                | 含义                                              |
| --------------------------------- | ----------------------------------------------- |
| `GetMesh()`                       | 获取当前 Actor 的 Mesh 组件（通常是 SkeletalMeshComponent） |
| `SetCollisionResponseToChannel()` | 设置对某个通道（Channel）的响应方式                           |
| `ECC_Visibility`                  | 通道名，表示**可见性通道**，用于射线检测（如鼠标点选、LineTrace）         |
| `ECR_Block`                       | 响应方式，表示“**阻挡该通道**”                              |

---

## 📌 效果说明：

> **让角色的 Mesh 对 `Visibility` 通道产生“阻挡”，从而可以被 `LineTraceByChannel()` 击中。**

例如：鼠标点选角色、LineTrace 查找敌人、射线检测都常用 `ECC_Visibility`。

---

## ✅ Unreal 的命名规则说明（以缩写命名为主）：

| 缩写     | 代表含义                                  | 示例                           |
| ------ | ------------------------------------- | ---------------------------- |
| `ECC_` | **E**ngine **C**ollision **C**hannel  | `ECC_Visibility`, `ECC_Pawn` |
| `ECR_` | **E**ngine **C**ollision **R**esponse | `ECR_Block`, `ECR_Ignore`    |

---

## ✅ 如何选择函数来调用？

### 🎯 函数选择指南（SkeletalMeshComponent/StaticMeshComponent 常用）：

| 场景                          | 使用函数                                  |
| --------------------------- | ------------------------------------- |
| 设置整体碰撞启用                    | `SetCollisionEnabled(...)`            |
| 设置对象通道（Pawn/WorldDynamic 等） | `SetCollisionObjectType(...)`         |
| 设置对某个通道的响应方式（单个）            | `SetCollisionResponseToChannel(...)`  |
| 设置所有通道的默认响应方式               | `SetCollisionResponseToAllChannels()` |
| 设置多个通道响应                    | `SetCollisionResponseToChannels(...)` |

# 17.Custom Depth & Stencil

在 Unreal Engine 中，**Custom Depth** 和 **Stencil** 是用于实现高级渲染效果（如描边、高亮、X 光等）的两个关键缓冲区。

---

## ✅ 1. Custom Depth 是什么？

**Custom Depth（自定义深度）** 是一张额外的深度图，用来：

* 储存**特定物体**的深度信息（非整个场景）
* 可在**后处理材质**中读取，用于比较和处理
* 不影响主渲染，只用于自定义效果

### 📌 常用功能：

* 描边
* 穿透高亮
* “透视墙体看敌人”效果（X-ray）

---

## ✅ 2. Stencil 是什么？

**Stencil（模板缓冲）** 是一个 **8 位整数缓冲区**，用于：

* 给每个物体**打标记（0\~255）**
* 在后处理材质中读取 `CustomStencil` 值，实现**分组控制**

### 📌 常用功能：

* 区分哪些对象需要特殊处理
* 多组描边（敌人红色、任务物品蓝色）
* 更精细控制哪些写入 Custom Depth 的物体被处理

---

## ✅ 二者配合使用（常见流程）：

| 步骤 | 操作                                     | 使用目的               |
| -- | -------------------------------------- | ------------------ |
| 1  | 启用 Render CustomDepth Pass             | 写入 Custom Depth 纹理 |
| 2  | 设置 Custom Stencil Value（如 1）           | 标记敌人、任务物体等         |
| 3  | 在材质中使用 SceneTexture\:CustomDepth       | 检测深度差做边缘检测         |
| 4  | 使用 SceneTexture\:CustomStencil + If 判断 | 只对 Stencil=1 的物体生效 |

# 18. 角色描边效果的实现

### 🧩 一、基本原理说明：

> “UE5 中常见的描边实现方式是通过 `CustomDepth` 和 `CustomStencil` 配合后处理材质实现的。核心思想是：将需要描边的物体写入一张额外的深度缓冲，再在后处理阶段检测其边缘像素并绘制描边颜色。”

---

### ⚙️ 二、实现步骤简述：

#### 1. 启用项目设置：

> 在项目设置（Project Settings）中启用 Custom Depth 和 Custom Stencil 通道：

* `CustomDepth-Stencil Pass`: Enabled with Stencil

#### 2. 设置需要描边的 Mesh：

> 对目标角色的 Mesh 启用自定义渲染：

```cpp
Mesh->SetRenderCustomDepth(true);
Mesh->SetCustomDepthStencilValue(1); // 设置组别
```

#### 3. 创建后处理材质：

> 创建一个 `Material`，类型设为 **Post Process**，使用节点如下：

* `SceneTexture: CustomDepth`：采样深度
* `SceneTexture: CustomStencil`：用于过滤特定物体
* `SceneTexture: PostProcessInput0`：原始画面
* `TexCoord` 偏移多个方向采样 → 比较深度 → 判定边缘
* `If` + `Lerp` 实现边缘上色逻辑

#### 4. 应用后处理材质：

> 在关卡中添加 PostProcessVolume：

* `Infinite Extent` 设置为 true（全局有效）
* 在 Post Process Materials 中添加材质实例

---

### 🎨 三、可扩展性（展示深度理解）：

> * 可以通过 `CustomStencil` 实现不同角色/物品使用不同描边色
> * 支持运行时启用/禁用描边（通过代码控制 `SetRenderCustomDepth`）
> * 可调整采样距离控制描边粗细
> * 与 UI、锁定系统结合，做出互动高亮反馈

---

### 💬 四、（可选）总结语句：

> “这种方式性能开销低、通用性强，是项目中常用的描边方案，也适合扩展到 X 光透视、目标高亮等效果。”

# 19. GAS
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250509184118612.png)

![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250509201413946.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250509202105091.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250509211117256.png)
# 20. GAS 多人联机优势（完全复制？）
Gameplay Ability System（GAS）在 **网络联机（Multiplayer）** 开发中具有以下 **显著优势**，是它被广泛应用于 UE 多人游戏项目的核心原因：

---

## ✅ 核心优势一览

| 优势                      | 说明                                                  |
| ----------------------- | --------------------------------------------------- |
| ✅ **自动支持网络同步**          | GAS 自带 RPC、属性同步、能力同步机制（基于 `AbilitySystemComponent`） |
| ✅ **客户端预测（Prediction）** | 减少延迟感，客户端可预测技能行为；失败时自动回滚                            |
| ✅ **属性复制优化**            | 属性通过 `AttributeSet` 精细同步，避免粗暴的 `replicate all`      |
| ✅ **服务器权威执行能力**         | 核心逻辑（应用效果、冷却、生效时间等）强制在服务端判定，防止作弊                    |
| ✅ **支持多玩家/多实体能力管理**     | 每个角色可拥有自己的 ASC（AbilitySystemComponent），独立管理技能和状态    |

---

## 🎮 举个例子：多人战斗场景

假设你有一个技能“冲锋斩”：

* 玩家在客户端按键触发
* GAS 会先进行 **客户端预测播放动画**
* 同时向服务器发送请求
* 服务器校验输入 → 真正调用 `ActivateAbility` → 触发效果（如伤害、击退）
* **GAS 自动处理**：

  * 效果同步（Gameplay Effect）
  * 伤害数值广播
  * Buff 应用状态同步
  * 技能冷却生效

---

## 📡 与传统手动实现相比

| 对比点        | 手动实现             | GAS 实现                    |
| ---------- | ---------------- | ------------------------- |
| 网络同步控制     | 手写 RPC/RepNotify | 自动绑定属性和技能同步逻辑             |
| 客户端预测      | 需自己处理预测/回滚       | 内置 Prediction Window 支持   |
| 属性/Buff 管理 | 需手写状态管理系统        | 使用 GameplayEffect 自动管理    |
| 安全性        | 易被客户端欺骗          | GAS 强制关键逻辑服务端执行           |
| 扩展性        | 增加新技能需加很多判断      | 只需添加 GA（Gameplay Ability） |

---

## 🔒 安全性方面

GAS 的服务器权威模式 + 结构化属性系统：

* 防止客户端伪造伤害
* 防止 Buff、冷却作弊
* 所有效果必须通过服务端 `ApplyGameplayEffectToTarget`

---

## ✅ 总结一句话回答（面试可用）：

> “GAS 在多人联机下的最大优势是：提供完整的网络同步框架，包括属性复制、技能触发、客户端预测与服务端权威执行，大大减少了开发者手动处理 RPC 和作弊防护的复杂度。”

# 21. GAS::Ability Task
非常好，这两个问题都与 **Unreal Engine 的 Gameplay Ability System（GAS）** 密切相关，尤其是在实现技能逻辑时，**Ability Task 是核心工具**。我们来逐个解释：

---

## ✅ 一、异步任务（Async Task）是啥？

**异步任务**就是一种**不会立即返回结果、而是在“某个事件完成后”触发回调的操作**。

### 🔧 在 UE/GAS 中表现为：

* 等待动画播放完成
* 等待蒙太奇中断或结束
* 等待输入释放
* 等待延迟、冷却、Tag添加等

这些过程都需要“监听”，不能阻塞主线程 —— 就属于**异步任务**。

---

## ✅ 二、Ability Task 是啥？有什么用？

**Ability Task** 是 GAS 中用于管理异步事件的**专用工具类**，它继承自：

```cpp
UGameplayTask → UAbilityTask
```

### 📦 作用：

> ✅ 让你在 Ability（GA）中轻松处理异步行为，比如：
>
> * 播放动画
> * 等待玩家输入
> * 等待网络同步
> * 等待 Gameplay Event
> * 等待特定时间、Tag、生效效果等

---

### 🔧 常见的 AbilityTask 示例：

| 类名                                | 作用                         |
| --------------------------------- | -------------------------- |
| `UAbilityTask_WaitTargetData`     | 等待玩家选中目标（例如选地释放）           |
| `UAbilityTask_WaitGameplayEvent`  | 等待 Gameplay Event（如攻击命中触发） |
| `UAbilityTask_PlayMontageAndWait` | 播放动画蒙太奇并等待其结束              |
| `UAbilityTask_WaitInputPress`     | 等待玩家按下技能键                  |
| `UAbilityTask_WaitDelay`          | 等待一段时间再执行（例如技能延迟生效）        |

---

## 🧠 举个例子（动画播放并等待）

```cpp
UAbilityTask_PlayMontageAndWait* Task = 
    UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, "Attack", Montage, 1.0f
    );

Task->OnCompleted.AddDynamic(this, &UMyGA::OnMontageFinished);
Task->ReadyForActivation();
```

🔹 上述代码表示：

* 播放一个攻击动画
* 等动画播放完才继续走后续逻辑（如造成伤害）

---

## ✅ 总结一句话（面试可用）：

> “AbilityTask 是 GAS 中专门处理异步事件的工具类，它简化了如动画播放、目标选取、输入监听等异步逻辑的实现，并允许我们通过事件回调在操作完成后再继续能力流程。”

# 22.NetUpdateFrequency

```cpp
NetUpdateFrequency = 100.f;
```

是设置一个 **Actor 在网络中的更新频率**，单位是 **Hz（次/秒）**。

---

## ✅ 解释：

* `NetUpdateFrequency` 是 Unreal Engine 中用于控制网络同步频率的属性
* 它表示：**每秒向客户端发送 Actor 的属性/状态更新的次数**

---

## 🚦 适用场景：

| 对象类型   | 建议值        | 原因              |
| ------ | ---------- | --------------- |
| 角色/玩家  | 50\~100 Hz | 移动频繁，动画同步要求高    |
| 投掷物/子弹 | 20\~60 Hz  | 需要较快同步位置，但生命周期短 |
| 门、机关   | 1\~10 Hz   | 状态变化不频繁，低频足够    |

---

## ⚠️ 注意：

* 更高的频率 → 更**实时**，但**网络开销大**
* UE 还会结合 `MinNetUpdateFrequency` 和 `bNetUseOwnerRelevancy` 等参数优化同步

---

## ✅ 面试一句话总结：

> "`NetUpdateFrequency` 控制 Actor 每秒同步到客户端的次数，值越高越实时，但开销越大，通常用于角色或需要频繁同步的动态对象。"

# 23.GAS in Multiplayer
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250510150449819.png)

这张图说明了 **GAS（Gameplay Ability System）在多人联机（Multiplayer）中的工作模型和架构原则**，尤其强调了“**服务器的权威性（Server Authority）**”这个关键点。

下面我将对图中内容进行**详细扩展解读**：

---

## ✅ 一、多人网络架构模型（图左）

### 1. 服务器（Server）

* 在游戏中维护“权威状态”（Authoritative State）
* 所有**重要决策逻辑都在服务器端执行**
* 所有客户端都连接并同步到它

### 2. 客户端（Clients）

* 客户端可以**预测操作**（如输入响应、动画播放）
* 但实际决定如伤害、生效、冷却等必须交由服务器处理

图中显示了三个客户端：

* `Client 0`
* `Client 1`
* `Client 2`

他们都从 `Server` 获取最新的游戏状态或将操作请求发往服务器。

---

## ✅ 二、服务端类型（图右上）

### 🔸 Dedicated Server（专用服务器）

* 没有玩家
* 没有界面（不渲染画面）
* 运行在数据中心或独立主机上
* 最常用于正式发布游戏的多人模式

### 🔸 Listen Server（监听服务器）

* 本地玩家即为**服务端 + 客户端**
* 拥有“主机优势”：本地操作无延迟
* 适合小型/局域网测试或对战

---

## ✅ 三、GAS 如何依赖这一架构？

GAS 在多人联机中的设计 **完全围绕“服务器为权威”展开**：

| 机制    | GAS 实现方式                                        |
| ----- | ----------------------------------------------- |
| 技能激活  | Client 发请求 → Server 校验 → 正式激活（通过 RPC）           |
| 属性同步  | 属性系统（AttributeSet）仅服务端修改 → 自动通过 `replicated` 推送 |
| 效果应用  | GameplayEffect 只能由服务端发起应用                       |
| 客户端预测 | 通过 `PredictionKey` + 预测窗口机制，提升响应性               |

---

## ✅ 一句话总结：

> Unreal 的 GAS 在多人游戏中以“服务端为权威”为核心原则，客户端仅做输入与预测，所有重要行为（技能释放、生效、伤害、冷却）必须由服务器决定并同步，以保证公平、安全与一致性。

# 24.Replication
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250510150904418.png)
这张图进一步深入讲解了 GAS 在多人联机（Multiplayer）中的**变量同步（Replication）机制**，重点是解释：**哪些变量在哪里修改？如何同步？为什么不能直接改？**

---

## ✅ 一、整体架构回顾

>- **服务端（Server）为权威**：**拥有唯一 GameMode**，并控制所有变量（PlayerController、PlayerState、Pawn）的真实值,没有HUD或Widgets
>- **客户端（Client 0\~2）**： 拥有**自身**PlayerController的复制，**所有**PlayerState、Pawn的复制，拥有唯一自身的HUD或widgets

## ✅ 二、变量如何同步？

>- 每个客户端都拥有 P\_2 的一个“镜像版本”，**但只有服务端的是“真身”**
>- ❌ 客户端直接修改 replicated 变量是无效的（且是非法的）
>-  **所有关键变量的修改必须发生在服务端，客户端只能读取已同步的值。**


---

## ✅ 三、与 GAS 的关系？

GAS 的属性系统（AttributeSet）正是依赖这个机制：

| 操作                           | 发生在哪                 |
| ---------------------------- | -------------------- |
| 应用 GameplayEffect，造成伤害       | Server               |
| 修改 Attribute（如 Health）       | Server               |
| 同步属性变化（Replicate Attributes） | Server → Client 自动复制 |
| Widget 获取变量显示                | Client 读取复制值         |

---

## ✅ 四、HUD 与 Widget 是怎么更新的？

每个客户端的 UI（Widget）会从本地副本读取数据：

```cpp
P_2->GetAttributeSet()->GetHealth();  // 这时是读取 replicated 值
```

通过绑定委托（OnRep\_）、监听 GAS 事件、或 Widget 更新 Tick 来完成 UI 更新。

# 25. Replication Mode
当然可以。下面我将用通俗易懂的方式，通过**三种典型游戏类型**来举例说明 GAS 的三种 Replication Mode 的使用场景和理由，帮助你建立更具象的理解：

---

## ✅ 1. `Full` 模式

🎮 **示例游戏**：
1. 单人 RPG（如《巫师3》、《塞尔达：荒野之息》）
2. 在某些观战视角中，比如《英雄联盟》的解说视角或回放系统，你可以完整看到所有角色的血量、Buff 状态、护盾值等。每个客户端都需要完整信息，用于精确展示战斗状态、计算预测或生成复盘。

### 📌 场景：

* 游戏只有一个玩家，所有 Gameplay Effect（例如中毒、燃烧、回血、护盾）都需要让玩家本地系统完整感知
* 不考虑网络流量，不存在同步优化问题

### 🧠 为什么用 Full？

* 玩家和系统逻辑运行在本地 → 所有效果、状态都要原原本本地执行和展示
* 更重要的是：**GAS 的复制机制不会被触发，反而更简单！**

✅ 适合：**单人游戏 或 离线训练模式**

---

## ✅ 2. `Mixed` 模式

🎮 **示例游戏**：多人对战类（如《英雄联盟》、《守望先锋》、《堡垒之夜》）

### 📌 场景：

* 玩家释放技能：命中后触发中毒、眩晕、减速等效果
* 自己需要感知完整的状态（如自己的冷却、受到的伤害值）
* 其他玩家只需要感知一些表现（如他晕了、身上冒绿光），**不需要知道详细数值**

### 🧠 为什么用 Mixed？

* GE 只复制给自己 → 降低网络压力
* Gameplay Tags（如“Stunned”）和 Cue（如爆炸特效）同步给所有人 → **状态可感知**
* 避免敌人看到你的具体 Buff 数值，提升游戏公平性和性能

✅ 适合：**多人游戏中“玩家控制角色”**

---

## ✅ 3. `Minimal` 模式

🎮 **示例游戏**：PVE 动作/射击游戏（如《无主之地》、《命运2》里的 AI 敌人）

### 📌 场景：

* 场景中大量 AI 怪物，攻击、回血、掉血都频繁
* 玩家看到的只是怪物血条减少、播放受伤动画
* 无需将每个 GE（伤害值、护盾计算、DOT 逻辑）都同步给每个客户端

### 🧠 为什么用 Minimal？

* 减少带宽消耗，**提升大规模 AI 战斗性能**
* GE 不复制（属性伤害、Buff 本地运行）
* 仅同步 Cue（特效）和 Tags（如“正在燃烧”）

✅ 适合：**AI 控制角色、大量非重要单位**

---

## ✅ 总结对比（一句话）

| 模式      | 应用示例                | 特点                       |
| ------- | ------------------- | ------------------------ |
| Full    | 单人游戏，如《塞尔达》；观战模式         | 所有内容完全复制，开发最简单           |
| Mixed   | MOBA/FPS，如《LOL》《OW》 | 自己完全感知，别人只看到表现，性能与表现兼顾   |
| Minimal | 大型副本 AI，如《无主之地》怪物   | 效果本地运行，别人只看到燃烧/受伤状态，性能最优 |

# 26.OwnerActor/AvatarActor/
带有ASC的Actor也被称为ASC的OwnerActor。ASC实际作用的Actor叫作AvatarActor。OwnerActor和AvatarActor可以是同一个Actor，比如MOBA游戏中的野怪。它们也可以是不同的 Actors，比如MOBA游戏中玩家和AI控制的英雄角色，OwnerActor是PlayerState、AvatarActor是HeroCharacter。大部分情况下OwnerActor和AvatarActor可以是角色Actor。不过想像一下你控制的英雄角色死亡然后重生的过程，如果此时要保留死亡前的Attributes或者GameplayEffects，那么最理想的做法是将ASC交给PlayerState。

如果OwnerActor和AvatarActor是不同的Actors，那么两者都需要实现IAbilitySystemInterface， 方便用于获取GAS的数据。这个接口只有一个方法需要被重载UAbilitySystemComponent* GetAbilitySystemComponent() const，此方法将返回ASC。

>- InitAbilityActorInfo设置OwnerActor/AvatarActor分为三种情况，分别需要在Server和Client端（即黑框内函数）中设置：
>1. **对于玩家控制的角色，ASC存在于Pawn中**，我通常在Pawn的 PossessedBy()方法中完成ASC在服务器端的初始化，在PlayerController的AcknowledgePawn()方法中完成ASC在客户端的初始化。
>2. **对于玩家控制的角色，ASC存在于PlayerState中**，我通常在Pawn 的PossessedBy() 方法中完成ASC在服务器端的初始化（这一点与上述相同），在 Pawn的 OnRep_PlayerState()方法中完成ASC在客户端的初始化（这将确保PlayerState在客户端已存在）。
>3. **对于AI控制的角色，ASC存在于Pawn中**，通常在Pawn的 BeginPlay()方法中完成ASC在服务器端和客户端的初始化。

![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250511154119081.png)
>- 注意：使用Mixed Replication Mode，OwnerActor 必须是Controller（但本项目不用担心：playState和Pawn（在PossessedBy中）的Owner会自动被设置为Controller）
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250511165356846.png)

# 27.Prediction
## ✅ 一句话总结（面试可用）：

> “Prediction 是 GAS 用于提升玩家操作响应的机制，客户端会提前执行操作（如播放动画），并等待服务器确认后决定是否保留或回滚，保证手感流畅同时保持网络安全。”

## 🔁 Prediction 的流程简图：

```plaintext
Client 按键 → 本地预测执行（播放动画等）
         ↓
    同时发送激活请求 → Server
                         ↓
             Server 判定是否合法
                         ↓
                ✅ → 确认并广播同步
                ❌ → 回滚客户端预测效果
```
---
## ✅ GAS 中常见预测应用：
| 预测行为     | 类别                                       |
| -------- | ---------------------------------------- |
| 播放攻击动画   | `AbilityTask_PlayMontageAndWait` 中支持预测播放 |
| 技能释放     | `TryActivateAbility` 可开启预测窗口             |
| 目标选择     | `TargetActor` 支持预测数据同步                   |
| 属性变化（可选） | 使用 `PredictionKey` 包装属性应用操作              |
---
## ⚠️ 注意点：
* 预测行为只在 **局部客户端可见**，不能直接影响服务器或其他客户端
* 必须手动管理预测窗口、确认或回滚逻辑
* GAS 自动支持了很多预测细节（如任务/GE 执行等）

# 28.RepNotify

## ✅ 一句话总结（面试可用）：
> “RepNotify 是 UE 的变量同步回调机制，用于当客户端接收到服务端同步的变量变化时，自动触发处理逻辑，常用于更新 UI、播放效果等视觉反馈。”

## 🧠 用途：
* 当客户端接收到 `Health` 的新值时，自动执行 `OnRep_Health()` 函数
* 通常用于更新 UI、播放动画、触发特效等客户端行为
---

## 🧪 示例：
```cpp
UPROPERTY(ReplicatedUsing = OnRep_Health)
FGameplayAttributeData Health;
```
```cpp
UFUNCTION()
void OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    // 比如更新 HUD 上的血条显示
    UpdateHealthBarUI();
}
```

# 29.AttributeSet
>- 在OwnerActor的构造方法中创建的AttributeSet将会**自动注册**到ASC。这一步必须在C++中完成。
>- Attributes 通常只能被GameplayEffects 修改，因此ASC可以 预测 这个修改。
>- 一个Attribute 由两个值构成 :一个基值 BaseValue 和一个当前值CurrentValue. 基值BaseValue是属性 Attribute的一个恒值， 而当前值 CurrentValue 是 BaseValue 加上GameplayEffects的临时修改值。
>- 立即（Instant） GameplayEffects将永久改变BaseValue，而持续（Duration） 和永恒（Infinite） GameplayEffects 将改变CurrentValue。周期性（Periodic ）GameplayEffects像立即（Instant） GameplayEffects一样将改变BaseValue。
如果Attribute没有受到GameplayEffects的影响时，这两个值其实是相同的。而受到GameplayEffects影响时，如果是一次性的修改，则直接修改的时候BaseValue，CurrentValue也会跟着改变，两个值的结果相同。如果影响是时效性的，比如一段时间内提高移动速度，那么，GameplayEffects修改的是CurrentValue的值。


![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250511192356289.png)
## 设置属性模板
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250511205008429.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250511205154964.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250512193233967.png)

# 30.联机复制
---

## ✅ 总结（一句话）：

> `GetLifetimeReplicatedProps` 告诉引擎“哪些变量要同步”，而 `OnRep_XXX()` 是当这些变量被同步到客户端后，执行的“后续反应逻辑”。


## ✅ 一、`GetLifetimeReplicatedProps` 的作用与原理

### 📌 作用：

告诉 Unreal Engine 哪些变量需要在 **服务器和客户端之间同步（Replicate）**，以及如何同步。

### 🔧 示例中这段代码：

```cpp
DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
```

### ✅ 参数解释：

| 参数                  | 含义                          |
| ------------------- | --------------------------- |
| `UAttributeSetBase` | 拥有变量的类名                     |
| `Health`            | 需要同步的变量                     |
| `COND_None`         | 同步条件（None = 始终同步）           |
| `REPNOTIFY_Always`  | 是否触发 `OnRep_Health()`（始终触发） |

### 📌 工作原理：

* 服务端修改 `Health`
* UE 根据 `DOREPLIFETIME` 注册的规则将新值同步到客户端
* 如果值发生变化，并且通知条件成立，就触发 `OnRep_Health()`

---

## ✅ 二、`OnRep_Health()` 的作用与原理

### 📌 作用：

在客户端 **接收到服务器同步的变量变化**时，触发回调，用于：

* 更新 UI
* 播放动画
* 调用特效等

### 🔧 示例中这段代码：

```cpp
void UAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Health, OldHealth);
}
```

其中：

```cpp
GAMEPLAYATTRIBUTE_REPNOTIFY(...)
```

是 GAS 提供的宏，内部会触发：

* `OnHealthChanged()` 委托
* 更新绑定的监听器（如 UI 蓝图、Widget）

---

## 🔁 二者的联动流程图如下：

```plaintext
服务端：Health 被 GE 修改
  ↓
调用 GetLifetimeReplicatedProps 注册的规则
  ↓
变量通过网络复制到客户端
  ↓
客户端触发 OnRep_Health()
  ↓
更新 UI / 动画 / 特效
```

# 31.`ATTRIBUTE_ACCESSORS`宏封装
## ✅ 宏总览：`ATTRIBUTE_ACCESSORS`

```cpp
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
```

它会自动展开为以下四个宏，四个宏又分别展开为四个对应的函数：
```cpp
static FGameplayAttribute GetHealthAttribute();
float GetHealth() const;
void SetHealth(float NewVal);
void InitHealth(float NewVal);
```
---

## ✅ 总结一句话：

> `ATTRIBUTE_ACCESSORS` 宏组提供了一整套对 GAS 属性的统一访问接口，包括：获取属性标识、读取当前值、设置值、初始化值，用于提升 GAS 开发效率并统一风格。
---

## 🎯 背景：为什么写这些宏？

在 GAS 中，我们经常需要重复写以上四个函数，而 GAS 提供了宏（Macro）来帮你统一自动生成这些函数，避免手动重复代码。

# Debug1.PlayerState为nullptr
#### 原因：场景内已有角色
正常情况下通过玩家出生点生成才是正确的做法，当然如果偏要在场景里面放也行，你放进去的角色，在细节面板里把Pawn分类下的自动控制玩家设置一下就行了，如果设置的是玩家0就只会有一个你操控的角色，设置成其他就会出现另一个角色
