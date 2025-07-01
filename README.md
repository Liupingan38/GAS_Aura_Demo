# 0.Gameplay框架
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250619205953445.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250619211447742.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250619212227544.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250619212404598.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250619213705007.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250627122627592.png)
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
将 Abstract 添加到 UCLASS 宏中用于标记该类为 **抽象基类**，禁止直接实例化;

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

# 6.✅ 前向声明

## ✅ 建议的做法：.h**前向声明 + cpp 中包含头文件**

### 原因：

1. **加快编译速度（最重要）**：

   * 使用前向声明（`class UAttributeInfo;`）只告诉编译器有这么一个类的声明。
   * 避免了 `.h` 文件中不必要的头文件依赖传递（Header Inclusion Bloat），从而提高编译效率，尤其在大型项目中尤为明显。

2. **降低耦合**：

   * 你的 `.h` 文件中只是声明了一个指针 `TObjectPtr<UAttributeInfo>`，编译器不需要知道 `UAttributeInfo` 的完整定义，因此前向声明足够。
   * 只有在 `.cpp` 文件中访问其成员时，才需要包含实际定义（也就是 `#include "AbilitySystem/Data/AttributeInfo.h"`）。

3. **避免循环依赖**：

   * 如果 `UAttributeInfo` 又引用了本类或其他你正在 include 的类，使用前向声明可以避免头文件间的循环包含问题。



| 方案                        | 优点           | 缺点                |
| ------------------------- | ------------ | ----------------- |
| **前向声明 + .cpp 中 include** | 编译快、耦合低、结构清晰 | 需要注意不能在 .h 中用完整类型 |
| **直接在 .h include**        | 快速开发，简单粗暴    | 编译慢、耦合高、易出循环依赖问题  |

> **最佳实践**：除非你在 `.h` 文件中需要使用完整类型（如继承、声明值类型变量、使用类型中的函数），否则一律使用前向声明 + cpp include。

---

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



### 含义：

* 虚函数支持**多态**（运行时绑定），可以不用override
* 父类指针或引用调用时，**会执行子类的重写版本**

```cpp
class Base {
public:
    virtual void Foo() {
        cout << "Base Foo" << endl;
    }
};

class Child : public Base {
public:
    void Foo() override {
        cout << "Child Foo" << endl;
    }
};

Base* b = new Child();
b->Foo();  // 输出 "Child Foo"
```

 **输出结果总览**：
   | 代码形式                  | 输出结果       | 是否合法 |
   |---------------------------|----------------|----------|
   | `Base* b = new Child();`  | `"Child Foo"`  | ✔️ 合法  |
   | `Base* b = new Base();`   | `"Base Foo"`   | ✔️ 合法  |
   | `Child* b = new Child();` | `"Child Foo"`  | ✔️ 合法  |
   | `Child* b = new Base();`  | **编译错误**   | ❌ 非法，无法通过子类指针指向父类对象  |


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

* **GE 只复制给自己** → 降低网络压力
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
* **GE 不复制**（属性伤害、Buff 本地运行）
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

# Debug：运行游戏，按一下~，输入showdebug abilitysystem
# Debug1.PlayerState为nullptr
#### 原因：场景内已有角色
正常情况下通过玩家出生点生成才是正确的做法，当然如果偏要在场景里面放也行，你放进去的角色，在细节面板里把Pawn分类下的自动控制玩家设置一下就行了，如果设置的是玩家0就只会有一个你操控的角色，设置成其他就会出现另一个角色


# 32. 动态委托系统

## 🧠 一句话理解

**动态委托 = 运行时可绑定的函数指针（并且支持蓝图）**

你可以：

* 把某个函数绑定到事件上（例如碰撞、触发器）；
* 当事件发生时，这个函数就会被自动调用。

---

## 🧩 动态委托的基本用法

### 1. 绑定方式

```cpp
Sphere->OnComponentBeginOverlap.AddDynamic(this, &AActorClass::OnOverlap);
```

### 2. 要求被绑定的函数必须这样写：

```cpp
UFUNCTION()
void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
```

### 💡 注意事项：

* 必须加上 `UFUNCTION()` 宏；
* 参数必须和事件要求的一致；
* 返回值必须是 `void`；
* 被绑定的函数不能是 `static`，不能是 `private`（除非 friend），否则会报错；
* `AddDynamic()` 是为**动态委托**使用的，和 `AddLambda()`、`AddUObject()` 是不同的。

---

## 🔁 对比静态委托 vs 动态委托

| 特性        | 静态委托（普通 C++ 委托）          | 动态委托（支持反射）   |
| --------- | ------------------------ | ------------ |
| 蓝图可访问     | ❌ 不可访问                   | ✅ 可访问        |
| 性能        | ✅ 更快                     | ❌ 稍慢（因为使用反射） |
| 持久化（保存状态） | ❌ 不可保存                   | ✅ 可被序列化保存    |
| 示例函数      | `AddUObject`、`AddLambda` | `AddDynamic` |
| 用法场景      | 游戏逻辑、C++ 系统事件            | 蓝图事件响应、组件触发  |

---


## 🧩 常用动态委托类型（在组件里）

| 委托名                        | 描述              |
| -------------------------- | --------------- |
| `OnComponentBeginOverlap`  | 某组件被另一个物体触发时调用  |
| `OnActorBeginOverlap`      | 整个 Actor 被触发时调用 |
| `OnComponentHit`           | 碰撞事件（如刚体碰撞）     |
| `OnDestroyed`              | Actor 被销毁时调用    |
| `OnClicked` / `OnReleased` | UI 或 Actor 被点击时 |

# 33. UFUNCTION()

`UFUNCTION()` 是 Unreal Engine（UE）中用于标记函数的宏，它的作用是让函数**参与 UE 的反射系统（Reflection System）**，从而支持：

---

## ✅ 1. **蓝图调用与事件绑定**

只有加了 `UFUNCTION()` 的函数才能：

* 被 **蓝图调用**；
* 被 **动态委托（`AddDynamic`）绑定**，没有 `UFUNCTION()`，编译器会报错；
* 被引擎自动识别、序列化、网络复制等。

## ✅ 2. **常用参数**
| 参数                              | 作用                            | 
| ------------------------------- | ----------------------------- |
| **BlueprintCallable**           | 允许在蓝图中调用该函数                   | 
| **BlueprintPure**               | 蓝图中可调用，且**不修改任何数据**（无副作用）     | 
| **BlueprintImplementableEvent** | 只声明，不在 C++ 中实现，**由蓝图实现**      | 
| **BlueprintNativeEvent**        | 可在 C++ 实现，也能被蓝图重写             | 
| **Exec**                        | 可在控制台中调用（如 `CheatManager` 命令） | 
| **Server**                      | 声明该函数只在服务端执行（网络同步）            | 
| **Client**                      | 声明该函数只在客户端执行（网络同步）            | 
| **NetMulticast**                | 在服务端调用，所有客户端同步执行              | 
| **Reliable**                    | 网络调用时保证传输（否则可能丢包）             | 
| **Unreliable**                  | 网络调用可丢弃（更快）                   | 
| **Category**                    | 设置蓝图中函数的分类                    | 

# 34.UCLASS 和 UPROPERTY


## 🔶 一、`UCLASS()` 的常用参数

`UCLASS()` 是用于声明一个类为 UE 可反射的类（必须继承自 `UObject` 或其子类，如 `AActor`、`UActorComponent` 等）。

### 🔹 常用参数列表

| 参数                   | 说明                                    |
| -------------------- | ------------------------------------- |
| `Blueprintable`      | 允许从该 C++ 类派生蓝图                        |
| `BlueprintType`      | 允许该类在蓝图中作为变量类型                        |
| `NotBlueprintable`   | 禁止从该类派生蓝图                             |
| `EditInlineNew`      | 支持在编辑器中内联创建此类的实例（通常用于 UObject 属性）     |
| `Abstract`           | 声明该类为抽象类，不能实例化                        |
| `Config=xxx`         | 指定配置文件类别，变量会自动绑定配置文件（如 `Config=Game`） |
| `DefaultToInstanced` | 默认情况下该类的对象会以 Instanced（实例化）方式创建       |
| `Within=OuterClass`  | 指定该类只能在特定 Outer 类中创建                  |



## 🔷 二、`UPROPERTY()` 的常用参数

`UPROPERTY()` 用于声明一个变量可以被反射系统识别，从而支持蓝图编辑、网络复制、垃圾回收等。

### 🔹 1. 蓝图相关参数

| 参数                   | 说明                         |
| -------------------- | -------------------------- |
| `BlueprintReadOnly`  | 蓝图中只读                      |
| `BlueprintReadWrite` | 蓝图中可读写                     |
| `VisibleAnywhere`    | 编辑器可见，但不可编辑                |
| `EditAnywhere`       | 编辑器中可见且可编辑（任意实例）           |
| `EditDefaultsOnly`   | 只能在默认对象上编辑（比如蓝图 Class 默认值） |
| `EditInstanceOnly`   | 只能在实例上编辑（运行时实例）            |
| `Category="xxx"`     | 设置蓝图变量分类                   |

| EditDefaultsOnly优点                | 缺点          |
| ----------------- | ----------- |
| ✅ 防止实例误改，统一管理属性设置 | ❌ 无法对单独实例调整 |
| ✅ 有助于设计数据结构清晰     |             |

### 🔹 2. 编辑器行为

| 参数                                          | 说明                    |
| ------------------------------------------- | --------------------- |
| `meta = (ClampMin = "0", ClampMax = "100")` | 设置浮点变量可调范围            |
| `DisplayName = "Health Points"`             | 编辑器中显示的变量名            |
| `ToolTip = "This is the player's health"`   | 鼠标悬浮提示                |
| `ExposeOnSpawn`                             | 支持在 SpawnActor 时设置该属性 |

### 🔹 3. 实例与内联相关

| 参数           | 说明                             |
| ------------ | ------------------------------ |
| `Instanced`  | 该 UObject 属性应实例化为独立对象（而不是指针共享） |
| `EditInline` | 在编辑器中支持内联编辑该 UObject           |
| `Transient`  | 不会保存到磁盘（关卡或序列化时忽略）             |
| `Config`     | 与配置文件联动，变量值自动从 `.ini` 加载       |

### 🔹 4. 网络复制与 GC

| 参数                               | 说明                   |
| -------------------------------- | -------------------- |
| `Replicated`                     | 网络中此变量会被复制到客户端       |
| `ReplicatedUsing=OnRep_Function` | 复制后调用的回调函数           |
| `SaveGame`                       | 会被保存到 SaveGame 文件中   |
| `VisibleInstanceOnly`            | 实例可见但不可修改            |
| `AdvancedDisplay`                | 编辑器默认隐藏，需要点开“高级”才能看到 |




# 34. 反射系统

Unreal Engine（UE）的\*\*反射系统（Reflection System）\*\*是整个引擎最核心的机制之一，支撑了蓝图（Blueprint）、编辑器属性暴露、网络复制、GC（垃圾回收）、动态委托等一整套生态。

---

## 🧠 一句话理解

**UE的反射系统让 C++ 能“像脚本语言一样”，在运行时查找、调用、修改类和变量。**

---

## 🔧 1. 反射的本质是什么？

在 C++ 中，类名、函数名、变量名在编译后都会消失（变成地址和汇编符号），我们不能像 Python、JavaScript 那样动态访问或调用。

Unreal Engine 通过 UHT（Unreal Header Tool）和一些宏（如 `UCLASS`, `UFUNCTION`, `UPROPERTY`）**提前生成元数据**，在编译时构建一个类型描述系统，使得运行时也可以：

* 查找某个类的函数名、变量名；
* 调用某个类的函数；
* 动态设置对象属性；
* 蓝图调用 C++ 函数；
* 进行网络同步；
* GC 自动回收 UObject；
* 实现动态委托；
* 编辑器显示和修改 C++ 对象属性。

---

## 🧩 2. 常见的反射相关宏

| 宏             | 用途                       |
| ------------- | ------------------------ |
| `UCLASS()`    | 声明类为可反射对象                |
| `USTRUCT()`   | 声明结构体为可反射                |
| `UENUM()`     | 声明枚举为可反射                 |
| `UFUNCTION()` | 声明函数可被反射（如蓝图调用、动态绑定）     |
| `UPROPERTY()` | 声明成员变量可被编辑器访问、GC追踪、网络复制等 |

---

### 🗃️ 示例代码：

```cpp
UCLASS()
class MYGAME_API AMyActor : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void DoSomething();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Health;
};
```

这段代码在编译时，UHT 会生成相应的元数据，供引擎查询使用。

---

## 🔍 3. 反射系统支持的功能一览

| 功能            | 是否基于反射                                                    |
| ------------- | --------------------------------------------------------- |
| 蓝图调用 C++ 函数   | ✅ 必须加 `UFUNCTION()`                                       |
| 蓝图读取/修改属性     | ✅ 必须加 `UPROPERTY()`                                       |
| 编辑器中显示变量      | ✅ `UPROPERTY(EditAnywhere)` 等                             |
| UObject 创建与查找 | ✅ 如 `FindObject<UClass>()`                                |
| 垃圾回收          | ✅ UObject 自动追踪所有 `UPROPERTY()` 成员                         |
| 网络复制（RPC）     | ✅ 需要 `UFUNCTION(Server/Client)` 和 `UPROPERTY(Replicated)` |
| 动态委托绑定        | ✅ 必须是 `UFUNCTION()` 才能 `AddDynamic()`                     |
| 蓝图事件/接口/组件系统  | ✅ 完全依赖反射数据                                                |
| 控制台命令调用函数     | ✅ 需加 `UFUNCTION(Exec)`                                    |

---

## 🧠 4. 元数据背后的机制

### 📍 UHT：Unreal Header Tool

* 在你每次编译 `.h` 文件时，UHT 会扫描你写的 `UCLASS`、`UFUNCTION` 等宏。
* 它会生成 `.generated.h` 文件，这些文件包含注册用的元信息，比如：

  * 类名、属性名、函数名
  * 每个类有哪些变量
  * 每个函数的参数与返回类型
  * 哪些变量可复制、可序列化

这些信息组成了 UE 的“反射数据库”，供运行时查询。

UE 使用一套自定义的编译器工具 —— UnrealHeaderTool (UHT)，在你编译项目时：
1. 扫描 .h 文件中的 UCLASS、UFUNCTION、UPROPERTY 宏；
2. 自动生成辅助代码（比如 .gen.cpp 文件）；
3. 让类、函数、变量注册到 UClass, UFunction, UProperty 等结构中；
4. 支持运行时通过名字查找类和成员，

---

## 🧱 5. UObject 是反射的基础单位

UE 的反射系统只适用于继承自 `UObject` 的类。

### 所以：

* 你不能在普通 `struct` 或标准 C++ 类中使用反射；
* 想用 `UPROPERTY` / `UFUNCTION` 等功能，必须继承 `UObject`（或其子类如 `AActor`, `UActorComponent`, `UUserWidget` 等）；

---

## 🎯 小结：UE 反射系统作用总结

| 能力     | 说明                                            |
| ------ | --------------------------------------------- |
| 类型识别   | `UClass::FindClass`, `IsA<T>()`, `GetClass()` |
| 属性控制   | 蓝图访问变量、编辑器调节属性                                |
| 动态函数调用 | 蓝图调用、动态委托、控制台命令                               |
| 生命周期管理 | 垃圾回收依赖 `UPROPERTY()`                          |
| 网络同步   | 属性复制和远程过程调用（RPC）                              |
| 数据序列化  | 存档/读取 SaveGame，自动保存变量                         |

# 35. const_cast

`const_cast` 是 C++ 中的一个类型转换运算符，它的**唯一作用**就是：

> **移除对象的 `const` 或 `volatile` 限定符**，从而使其可以被修改（或用于需要非 `const` 的场景）。（**你得自己保证这样做是安全的**）。



## 🧠 对比其他类型转换

| 转换                 | 用途                      | 是否安全          | 是否运行时检查 |
| ------------------ | ----------------------- | ------------- | ------- |
| `static_cast`      | 常规类型转换                  | 安全（编译时检查）     | ✅       |
| `reinterpret_cast` | 强制转换（低层次）               | 极不安全          | ❌       |
| `dynamic_cast`     | RTTI运行时类型转换（通常用于多态类）    | 安全（失败返回 null） | ✅       |
| `const_cast`       | 去掉 `const` 或 `volatile` | ⚠️ 危险         | ❌       |

# 36.static_class()



## 🧠 一句话回答：

> `StaticClass()` 返回的是 **这个类的类型信息（`UClass*`）**，用于告诉引擎你想要获取哪一类的 `AttributeSet`。

```cpp
const UAuraAttributeSet* AuraAttributeSet =
	Cast<UAuraAttributeSet>(
		ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass())
	);
```


## 🧩 为什么要传 `UAuraAttributeSet::StaticClass()`？

### ✅ 因为 `GetAttributeSet()` 需要你指定 **你想取哪个属性集类型的对象**

* `AbilitySystemComponent` 可能有多个属性集（AttributeSet），比如：

  * `UHealthAttributeSet`
  * `UManaAttributeSet`
  * `UAuraAttributeSet`（你自定义的）
* 所以你要告诉它：**我要的是 `UAuraAttributeSet` 类型的那一个属性集实例。**

这时候就必须传入：

```cpp
UAuraAttributeSet::StaticClass()
```

它的作用是：

* 获取 UAuraAttributeSet 对应的 **类型对象**（`UClass*`）；
* 然后内部查找匹配的属性集并返回指针。

---

## 📌 `StaticClass()` 是什么？

在 UE 中，每个 `UCLASS()` 类型都会自动生成一个 `StaticClass()` 静态函数，用来返回它的类型信息对象（`UClass*`）。

等价于：

```cpp
UClass* AuraAttrClass = UAuraAttributeSet::StaticClass();
```
# 37.MVC
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250613162502886.png)
MVC（Model-View-Controller）是一种常见的软件架构模式，用于组织和设计应用程序。它将应用程序分为三个逻辑层：模型（Model）、视图（View）和控制器（Controller）。

模型（Model）层： 主要处理数据相关的内容，和数据库进行读取，写入，更新，删除等操作。并定义了操作和访问这些数据的方法。
视图（View）层： 视图层负责展示模型层的数据给用户，并接收用户的输入。它是用户界面的一部分，负责展示信息、呈现数据、收集用户输入等。视图层通常是根据模型层的数据来动态生成的，以便用户可以直观地与数据进行交互。
控制器（Controller）层： 控制器层接收用户的输入，并根据输入调度和处理请求。它负责处理用户与应用程序的交互逻辑，决定如何更新模型层数据和选择合适的视图层。控制器将用户的请求转发给模型层进行处理，并在完成后更新视图层以显示结果。
通过MVC的方式，我们将各部分模块化分工，将关注点分离，提高应用程序的可维护性，可扩展性和可测试性。

很好的问题！你提到的 **Slot** 和 **Overlay Slot** 是 Unreal Engine UMG（UI 系统）中的**布局系统核心概念**，下面我会通俗、准确地解释它们的含义及区别。

# 38.Slot布局插槽



> 在 Unreal UMG 中，**Slot 是 Widget 在父容器中的布局信息的封装**，包括了像 **对齐方式、边距、位置** 等属性。

也就是说，**每个控件（Widget）被添加到一个父容器中时，都会被自动包装在一个 Slot 中**，这个 Slot 的类型取决于它所在的容器类型。如overlay Slot就是在overlay父容器下的

# 39.`TSubclassOf`：

---

### 1. 定义

`TSubclassOf<T>` 是 Unreal Engine 提供的模板类型，用于保存一个类类型（`UClass*`），但**限制它必须是 `T` 或其子类**。它比直接使用 `UClass*` 更安全，具备类型检查功能，常用于在编辑器中指定蓝图类。

---

### 2. 使用场景

* **编辑器选择蓝图类**：例如在组件中让设计师选择一个继承自 `UUserWidget` 的蓝图用于 UI。
* **动态创建对象**：搭配 `CreateWidget<T>`、`SpawnActor<T>` 进行实例化时指定类。
* **数据驱动设计**：如技能系统、工厂模式、AI 行为系统中，用 `TSubclassOf` 来配置将来要实例化的类类型。

```cpp
UPROPERTY(EditAnywhere)
TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

UAuraUserWidget* Widget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
```

---

### 3. 注意事项

* **不等于类实例**，它保存的是类（`UClass*`），不能直接调用函数，必须先实例化。
* **只能传子类**，否则编译时报错；这比裸 `UClass*` 更安全。
* 若用在模板函数中（如 `CreateWidget`），模板参数必须与 `TSubclassOf` 的类型一致，否则类型推导失败。

# 40.类类型 ≠ 类实例


### 1. 定义

**实例化（Instantiate）** 是指：根据一个类的定义，**创建出具体的对象（实例）**，这样你才能使用它的函数和成员变量。

---

### 2. 使用场景举例

如果你有这个变量：

```cpp
TSubclassOf<UAuraUserWidget> OverlayWidgetClass;
```

它只是代表一个类，比如你选中了蓝图类 `WBP_HealthGlobe`，但它只是一个“类类型”，并不是一个真实的对象。

你不能这么做：

```cpp
OverlayWidgetClass->SetHealth(50.f); // ❌ 报错：类不能直接调用实例方法
```

你必须先通过 `CreateWidget` 把它实例化成一个对象：

```cpp
UAuraUserWidget* Widget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
Widget->SetHealth(50.f); // ✅ 正确：Widget 是对象，能调用方法
```

---

### 3. 注意事项

* `TSubclassOf<T>` 实质是 `UClass*` 的类型安全封装，不能当对象用；
* 要操作类中的成员（比如调用函数、访问属性），**必须通过引擎的工厂函数如 `CreateWidget` 或 `SpawnActor` 实例化**；
* 记住：**类只是蓝图，实例才是实体**，就像图纸不能当房子住，你得先建出来。

# 41.工厂函数

## 🔧 一句话定义：

> **工厂函数是一种用来“生成对象”的函数，它隐藏了对象创建的细节，只暴露一个清晰的接口。**

---

## 🧱 示例对比（以 Unreal 的 `CreateWidget` 为例）

### 🧱 普通方式（直接构造）：

你手动构造对象，写法通常是：

```cpp
UAuraUserWidget* Widget = NewObject<UAuraUserWidget>(...);
```

这种方式灵活，但需要你手动设置很多细节，比如生命周期、Outer、Class 类型等。

---

### 🏭 工厂函数方式（封装创建）：

Unreal 给你写好了一个工厂函数：

```cpp
UUserWidget* Widget = CreateWidget<UUserWidget>(World, WidgetClass);
```

这个函数内部自动帮你做了很多事情：

* 指定 Outer；
* 设置生命周期管理；
* 初始化 Widget；
* 保证和 UMG 系统集成良好。

你只用提供：

* `World`：在哪个世界创建；
* `WidgetClass`：用哪种 Widget 蓝图类。

---

## 🧠 为什么使用工厂函数？

| 优点          | 说明                                        |
| ----------- | ----------------------------------------- |
| ✅ **封装复杂性** | 用户不必关心构造细节，比如 GC、父对象、初始化顺序                |
| ✅ **减少出错**  | 比直接 `new` 或 `NewObject` 更安全               |
| ✅ **统一接口**  | 便于统一管理，如 UI 创建都通过一个函数                     |
| ✅ **支持多态**  | 可以返回一个基类指针，实际类型由传入的类决定（如 UUserWidget 是基类） |

---

## 🎮 Unreal 中常见的工厂函数示例：

| 工厂函数                        | 用途               |
| --------------------------- | ---------------- |
| `CreateWidget<T>()`         | 创建 UI Widget     |
| `NewObject<T>()`            | 创建任意 UObject 类型  |
| `SpawnActor<T>()`           | 在场景中生成 Actor     |
| `ConstructObject<T>()`（已弃用） | 老版本 UObject 工厂函数 |


# 42. 工厂方法模式 vs 抽象工厂模式



## 1️⃣ 工厂方法模式（Factory Method Pattern）

### ✅ 定义

> **定义一个创建对象的接口，但让子类决定要实例化的类。**
> 即：延迟对象的实例化到子类中完成。

---

### 🧱 结构

```cpp
class Product { public: virtual void Use() = 0; };

class ConcreteProductA : public Product { public: void Use() override {...}; };
class ConcreteProductB : public Product { public: void Use() override {...}; };

class Factory {
public:
    virtual Product* CreateProduct() = 0;
};

class ConcreteFactoryA : public Factory {
public:
    Product* CreateProduct() override { return new ConcreteProductA(); }
};

class ConcreteFactoryB : public Factory {
public:
    Product* CreateProduct() override { return new ConcreteProductB(); }
};
```

---

## 2️⃣ 抽象工厂模式（Abstract Factory Pattern）

### ✅ 定义

> 提供一个接口，用于创建**一系列相关或相互依赖的对象**，而不需要指定它们的具体类。

---

### 🧱 结构

```cpp
// 抽象产品
class Button { public: virtual void Render() = 0; };
class Checkbox { public: virtual void Render() = 0; };

// 具体产品
class WindowsButton : public Button { void Render() override {...}; };
class MacButton : public Button { void Render() override {...}; };
class WindowsCheckbox : public Checkbox { void Render() override {...}; };
class MacCheckbox : public Checkbox { void Render() override {...}; };

// 抽象工厂
class GUIFactory {
public:
    virtual Button* CreateButton() = 0;
    virtual Checkbox* CreateCheckbox() = 0;
};

// 具体工厂
class WindowsFactory : public GUIFactory {
public:
    Button* CreateButton() override { return new WindowsButton(); }
    Checkbox* CreateCheckbox() override { return new WindowsCheckbox(); }
};

class MacFactory : public GUIFactory {
public:
    Button* CreateButton() override { return new MacButton(); }
    Checkbox* CreateCheckbox() override { return new MacCheckbox(); }
};
```

---



## 🔍 总结对比

| 对比点      | 工厂方法模式         | 抽象工厂模式         |
| -------- | -------------- | -------------- |
| 创建产品数量   | 一种产品           | 多种产品           |
| 产品之间是否关联 | 无强关联           | 产品是一组、强关联（产品族） |
| 是否使用继承   | 子类实现工厂方法       | 接口组合多个工厂方法     |
| 扩展新产品    | 容易：新建一个子类工厂    | 困难：需改抽象工厂接口    |
| 常见用途     | 单一类型扩展，如不同技能组件 | UI 控件系统、游戏皮肤系统 |

---



## ✅ 举例说明（游戏场景）

### 工厂方法模式：

* 每种敌人（Zombie、Boss、Wolf）都有自己的工厂类。
* 创建敌人对象时，通过工厂调用 `CreateEnemy()`。

### 抽象工厂模式：

* 一整套 UI 样式（按钮、窗口、进度条）统一由 `FantasyUIFactory` 或 `SciFiUIFactory` 创建，保证样式一致。


# 43.**向下转型（downcast）**

```cpp
UAttributeSet* Attr = new UAuraAttributeSet(); // 基类指针指向子类对象
```

你后来想访问子类的函数（比如 `GetHealth()`），就需要把它转回子类类型：

```cpp
UAuraAttributeSet* AuraAttr = Cast<UAuraAttributeSet>(Attr);
AuraAttr->GetHealth(); // ✅ 子类特有的函数
```

只要**对象本身是子类的实例**，就可以安全地进行这种转换。

# 44.动态多播委托



> **动态多播是一种可以在蓝图中绑定多个响应函数的“事件代理（委托）”，在某个事件触发时，一口气调用所有绑定的函数。**


---

## ✅ 举个例子（代码）

### 假设你有一个角色，当血量变化时通知多个 UI 控件更新：

```cpp
// 在角色的头文件中
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);

UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // 事件声明
    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;
    
    void TakeDamage(float Damage) {
        Health -= Damage;
        OnHealthChanged.Broadcast(Health); // 广播消息
    }

private:
    float Health = 100.0f;
};
```

---

### 然后在蓝图中：

* 你可以从角色蓝图中绑定 `OnHealthChanged` 到多个 UI 元素，比如：

  * 更新血条
  * 播放受伤动画
  * 播放音效



## ✅ 为什么叫“动态”？

| 类型              | 意义                     |
| --------------- | ---------------------- |
| **动态（Dynamic）** | 支持蓝图中绑定、解绑（运行时可以动态连接）  |
| 静态（Non-Dynamic） | 仅限 C++ 中使用，效率更高但不可蓝图绑定 |

# 45.GetGameplayAttributeValueChangeDelegate

你贴出的代码是 Unreal Engine Gameplay Ability System（GAS）中用于**属性变化监听与事件广播**的标准做法。

## 🔍 详细拆解

### 🔧 1. `BindCallbacksToDependencies()`

```cpp
void UOverlayWidgetController::BindCallbacksToDependencies()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
```

```cpp
AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
    AuraAttributeSet->GetHealthAttribute())
    .AddUObject(this, &UOverlayWidgetController::HealthChanged);
```

这句话是**核心**：

* `GetGameplayAttributeValueChangeDelegate(...)` 是 GAS 提供的接口，返回一个代理对象（delegate）；
* `.AddUObject(...)` 表示 **当这个属性发生变化时，调用你的函数 `HealthChanged()`**；
* `this` 表示绑定在当前的 `UOverlayWidgetController` 实例上；


### 🔧 2. 响应函数（回调函数）

```cpp
void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
    OnHealthChanged.Broadcast(Data.NewValue);
}
```

* `Data.NewValue`：当前新血量；
* `OnHealthChanged` 是你在类中定义的动态多播代理；
* `.Broadcast(...)` 表示：通知所有监听这个事件的系统，比如 UI 血条！



---

## 🎯 为什么要这样写？

| 目的        | 实现方式                                    | 原因              |
| --------- | --------------------------------------- | --------------- |
| 监听属性变化    | `AddUObject(..., &X::Func)`             | GAS 提供的接口       |
| 解耦 UI 与数据 | `.Broadcast(...)`                       | 多个系统可以监听，无需知道彼此 |
| 蓝图可用      | 动态多播 + `UPROPERTY(BlueprintAssignable)` | 绑定蓝图响应函数        |
| 结构清晰      | 每个属性单独监听、单独处理                           | 易于维护和扩展         |

---

## ✅ 实际运行时会发生什么？

1. 角色受到伤害 → GAS 自动修改 `Health` 值；
2. `AbilitySystemComponent` 发现 `Health` 被改动；
3. 它触发 `GetHealthAttribute().Delegate → HealthChanged()`；
4. `HealthChanged()` 再广播 `OnHealthChanged(NewValue)`；
5. 绑定这个广播的 UI 蓝图更新血条数值。


# Debug2.多人模式下，AttributeSet为nullptr
#### 原因：和场内药水重叠了，先触发了overlay（）加血,但此时角色还未创建，AttributeSet为nullptr

# 46.日志
这行代码是 Unreal Engine（UE）中用来在控制台输出日志的宏，具体含义如下：

```cpp
UE_LOG(LogTemp, Warning, TEXT("Health:%f"), NewValue);
```

---

### ✅ 含义逐部分解释：

| 部分                  | 含义                                                                                        |
| ------------------- | ----------------------------------------------------------------------------------------- |
| `UE_LOG`            | UE 的日志输出宏，类似于 `printf` 或 `std::cout`，用于打印调试信息。                                            |
| `LogTemp`           | 日志分类（Category），`LogTemp` 是默认临时日志类别。如果你有自定义日志，可以替换成你自己的类别。                                 |
| `Warning`           | 日志级别。常见的还有：`Log`, `Warning`, `Error`，分别表示普通日志、警告、错误。                                      |
| `TEXT("Health:%f")` | 日志内容格式字符串。这里使用 `TEXT` 宏是为了兼容 Unreal 的宽字符系统（`FString`）。`%f` 是 C++ 中的格式符，表示输出一个 `float` 类型。 |
| `NewValue`          | 要替换 `%f` 的变量值，即打印出的数值。                                                                    |

---


### 🔧 提示：

如果你希望这些日志也能在屏幕上显示（而不仅仅是控制台），可以这样做：

```cpp
if (GEngine)
{
    GEngine->AddOnScreenDebugMessage(
        -1, 5.f, FColor::Red, FString::Printf(TEXT("Health: %f"), NewValue));
}
```

# 47.public\protected\private
## 🎮 在 UE5 GAS 开发中的应用建议

### 1. `public`

🔓 **对外暴露使用的变量或函数**，适合：

* 需要暴露给 **蓝图**、其他模块调用的接口
* 希望其他模块通过 `GetHealth()` 读取血量等数据
* 通常配合 `UFUNCTION(BlueprintCallable)` 或 `UPROPERTY(BlueprintReadWrite)` 使用

```cpp
public:
    UPROPERTY(BlueprintReadWrite, Category="Attributes")
    FGameplayAttributeData Health;

    UFUNCTION(BlueprintCallable)
    void ApplyHealing(float Amount);
```

📌 适用场景：提供 **公共接口**，供其他系统调用、蓝图访问等。

---

### 2. `protected`

🛡️ **子类可以继承访问，但不暴露给外部模块**，适合：

* 只希望子类能访问的变量/函数（如内部初始化、钩子函数）
* 一些模板方法模式的函数，例如 `ApplyEffectInternal()` 在 `ApplyEffect()` 中被调用

```cpp
protected:
    void HandleDeath();

    UPROPERTY()
    bool bIsDead;
```

📌 适用场景：需要 **类内部扩展性**，但不想暴露太多细节。

---

### 3. `private`

🔐 **仅本类自己能访问的实现细节**，适合：

* 临时状态、缓存、内部算法细节
* 不希望被子类或其他模块访问

```cpp
private:
    float CachedDamage;

    void UpdateInternalState();
```

📌 适用场景：**完全不对外暴露**的内部逻辑，封装性好。

---

## 💡 GAS 中常见习惯总结

| 元素类型                                | 推荐访问修饰符                                   | 原因         |
| ----------------------------------- | ----------------------------------------- | ---------- |
| `UPROPERTY()` 用于属性（如 Health、Mana）   | `public`（通常加 BlueprintReadWrite/ReadOnly） | 方便在蓝图中访问   |
| `AbilitySystemComponent` 的绑定逻辑等辅助函数 | `protected`                               | 子类可以调用     |
| 临时变量（如缓存值、计时器）                      | `private`                                 | 实现细节，不对外暴露 |
| BlueprintCallable 的函数（如对外接口）        | `public`                                  | 供蓝图和其他类调用  |

---

## 🚨 注意事项

* **蓝图访问权限 ≠ C++ 的 public**：即使变量是 `protected` 或 `private`，加了 `UPROPERTY(BlueprintReadWrite)` 也能在蓝图里访问。反射系统绕过了 C++ 的访问控制。
* **如果你不希望蓝图访问，记得移除 BlueprintXXX 宏**。

# 48.委托

## ✅ 一句话理解委托

> UE 的“委托”允许你注册多个函数，当特定事件发生时，这些函数会被自动调用。

---

## 📚 UE 中委托的四种类型（按功能）

| 类型                                   | 描述            | 是否支持多播 | 是否支持蓝图 |
| ------------------------------------ | ------------- | ------ | ------ |
| `DECLARE_DELEGATE`                   | 基础委托，不带参数     | ❌ 单播   | ❌ 否    |
| `DECLARE_DELEGATE_OneParam` 等        | 基础委托，带参数      | ❌ 单播   | ❌ 否    |
| `DECLARE_DYNAMIC_DELEGATE`           | 支持反射（如蓝图）     | ❌ 单播   | ✅ 是    |
| `DECLARE_DYNAMIC_MULTICAST_DELEGATE` | 支持反射+多播（最常用）  | ✅ 多播   | ✅ 是    |
| `DECLARE_MULTICAST_DELEGATE`         | C++ 多播，但不支持蓝图 | ✅ 多播   | ❌ 否    |


## 🛠 示例：玩家血量变化通知 UI

### 1️⃣ 在角色组件中声明委托

```cpp
// 定义委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);

UCLASS()
class MYGAME_API UMyHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChangedSignature OnHealthChanged;

	void ChangeHealth(float NewHealth);
};
```

### 2️⃣ 实现函数并触发委托

```cpp
void UMyHealthComponent::ChangeHealth(float NewHealth)
{
	// 修改内部血量
	CurrentHealth = NewHealth;

	// 触发通知（所有监听者都会收到）
	OnHealthChanged.Broadcast(CurrentHealth);
}
```

### 3️⃣ 蓝图中绑定

在蓝图中，选中该组件，可以使用事件图表绑定 `OnHealthChanged`，就像响应按钮点击一样。

# 49.lambda表达式
非常好，你提到的是 **C++11 的 Lambda 表达式** 在 Unreal Engine 项目中的典型应用，结合了 UE 的 GAS（Gameplay Ability System）和调试功能。

下面我会详细解释两部分内容：

---

## ✅ 一、什么是 Lambda 表达式？

### 🧠 本质理解：

> Lambda 是一种“**匿名函数**”——你可以**内联定义一个函数并立即使用它**，尤其适用于事件回调、临时逻辑。

---

### 🔹语法结构：

```cpp
[捕获列表](参数列表) -> 返回类型 { 函数体 }
```

但很多时候可以省略返回类型。

---

### 📌 示例解析你给的代码：

```cpp
Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
	[this](const FGameplayTagContainer& TagContainer)
	{
		for (const FGameplayTag& Tag : TagContainer)
		{
			// 打印标签
			const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Msg);

			// 查表获取信息（如用于 UI）
			GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
		}
	}
);
```

---

## ✅ 二、关键概念解释

### 1. `AddLambda(...)` 是什么？

这是一个 **将 lambda 表达式注册到委托中** 的写法，相当于告诉 `EffectAssetTags`：

> 以后当某个事件触发时，就调用这段匿名函数！

---

### 2. `[this]` 是捕获列表

* `this` 表示捕获当前类的 `this` 指针；
* 这样你就可以在 lambda 内部访问当前类的成员函数（如 `GetDataTableRowByTag()`）和变量（如 `MessageWidgetDataTable`）；

#### ✅ 示例类成员访问：

```cpp
[this] { this->DoSomething(); }
```

或者省略 `this->`：

```cpp
[this] { DoSomething(); }
```

### ✅ 捕获列表其他写法：

| 写法              | 含义                   |
| --------------- | -------------------- |
| `[=]`           | 捕获外部所有变量 **按值**（只读）  |
| `[&]`           | 捕获外部所有变量 **按引用**（可改） |
| `[this]`        | 捕获当前对象指针（用于类内成员访问）   |
| `[var1, &var2]` | 显式捕获某些变量，按值或按引用      |

---

## ✅ 三、应用场景

在 Unreal Engine 中你经常在以下场景看到 Lambda：

* `AddLambda()` 添加事件监听器（如按钮、计时器、GAS标签广播等）
* `AsyncTask`、`Timer` 等异步回调
* 在 `TFunction`、`std::function` 等需要函数对象的地方传入一段短逻辑

# 50.const成员函数和mutable

## ✅ `const` 成员函数含义

在 C++ 中，当你写一个成员函数时在结尾加上 `const`：

```cpp
ReturnType ClassName::FunctionName(...) const
```

这表示：

> **这个成员函数不会修改当前类对象的成员变量（也不能调用其他非 const 的成员函数）**。

---

## ✅ 为什么要用 `const`？

主要目的有两个：

### 1. **语义清晰**

你告诉调用者：“我这个函数只是查询数据，不会动任何东西。”

### 2. **支持 const 对象调用**

```cpp
const UAuraAbilitySystemComponent* Component = GetComponent();
Component->EffectApplied(...);  // OK 只有当 EffectApplied 是 const 函数
```

---


## ✅ 什么时候**不能**加 `const`？

你**不能把成员函数标为 `const`**，如果它：

1. ❌ **修改了类的成员变量**
2. ❌ **调用了非 `const` 成员函数**
3. ❌ **对成员变量调用了可能修改状态的非 `const` 方法**
4. ❌ **访问了不能用 `mutable` 标记的状态性资源（如日志缓存、计数器等）**

------

## 🛠 如何解决？

你有两种方式：

### ✅ 方法 1：**去掉 const**

如果函数本来就会改状态，那你就别写 `const`。

```cpp
void SetValue(int NewVal) { Value = NewVal; }
```

---

### ✅ 方法 2：**使用 `mutable`**

如果你修改的成员变量不代表逻辑状态（如缓存、调试信息），可以把它声明为 `mutable`：

```cpp
class MyClass
{
public:
    void LogAccess() const { AccessCount++; }

private:
    mutable int AccessCount = 0;
};
```

✅ 这样 `AccessCount++` 在 const 函数中是允许的。

---

## ✅ 总结表格

| 行为                       | 是否允许在 `const` 成员函数中 |
| ------------------------ | ------------------- |
| 修改成员变量（如 `Health = 100`） | ❌ 否                 |
| 调用非 const 的其他成员函数        | ❌ 否                 |
| 调用 const 的其他成员函数         | ✅ 是                 |
| 修改 `mutable` 成员变量        | ✅ 是                 |
| 调用成员对象的 `const` 方法       | ✅ 是                 |
| 调用成员对象的非 `const` 方法      | ❌ 否                 |

## ✅ 总结：返回值的 const 推荐策略
| 用法                       | 原因                    |
| ------------------------ | --------------------- |
| `const TArray<Type>&` 传参 | 避免复制数组（高性能）           |
| `const FString&` 传参      | 避免构造/析构字符串            |
| 成员函数加 `const`            | 可在 const 对象中调用；编译器能优化 |
| 返回 `const &`             | 避免复制 + 禁止修改           |

# 51.成员变量/函数/对象/指针
| 名称                      | 是什么          | 举例                       | 属于谁                   |
| ----------------------- | ------------ | ------------------------ | --------------------- |
| ✅ 成员变量（Member Variable） | 类里的变量，用于保存数据 | `int Health;`            | 属于类的每个对象              |
| ✅ 成员函数（Member Function） | 类里的函数，用于定义行为 | `void Attack();`         | 属于类的每个对象              |
| ✅ 成员对象（Member Object）   | 一个类作为另一个类的变量 | `Inventory MyBag;`       | 属于类对象的成员变量，但它本身是个“对象” |
| ✅ 成员指针（Member Pointer）  | 类中指向对象的指针    | `USceneComponent* Mesh;` | 属于类，但指向其他堆上的对象        |

# 52.`AActor` vs `APawn` vs `ACharacter`

## ✅ 类关系图（继承关系）

```
AActor
  ├── APawn
  │     └── ACharacter
```

---

## ✅ `AActor` vs `APawn` vs `ACharacter` 对比表

| 特性/类项                | `AActor`            | `APawn`                    | `ACharacter`                      |
| -------------------- | ------------------- | -------------------------- | --------------------------------- |
| 📚 继承层级              | 最基本的游戏对象类           | 继承自 `AActor`，代表“可被控制”的实体   | 继承自 `APawn`，为人形角色提供完整移动系统         |
| 👥 是否可被控制            | ❌ 默认不可（需扩展逻辑）       | ✅ 可以被 `Controller` 控制      | ✅ 同样可被控制                          |
| 🧠 控制器支持             | ❌ 无 `Controller` 支持 | ✅ 支持 AI 和 PlayerController | ✅ 同样支持                            |
| 🧱 是否具备运动组件          | ❌ 没有内建移动系统          | ❌ 需要自己实现移动组件               | ✅ 自带 `CharacterMovementComponent` |
| 🏃 是否支持走/跑/跳/下落      | ❌ 不支持               | ❌ 自己实现                     | ✅ 内建功能完善（含地面检测、重力）                |
| 🦴 是否自带 SkeletalMesh | ❌ 你得手动加             | ❌ 自己加                      | ✅ 默认含 `SkeletalMeshComponent`     |
| 🎯 是否有碰撞体            | ❌ 默认无               | ❌ 需手动添加                    | ✅ 默认 `CapsuleComponent` 作为 Root   |
| 🔂 Tick 支持           | ✅ 有 Tick            | ✅ 同上                       | ✅ 同上                              |
| 🧩 推荐用途              | 所有对象的基础，如门、武器、机关等   | 自定义可控对象，如车、飞船、炮塔           | 人形角色，如玩家、敌人、NPC                   |
| 🧠 AI 可控             | 需自定义行为              | ✅                          | ✅                                 |
| 🔁 动画蓝图支持            | ❌                   | ✅（加 SkeletalMesh 后）        | ✅（原生支持）                           |
| 🎮 玩家控制支持            | ❌ 需扩展               | ✅ 支持                       | ✅ 支持                              |

---

## ✅ 总结要点

* `AActor` 是最基础的游戏对象。
* `APawn` 用于需要被“控制”的对象（玩家、AI），但不自带移动能力。
* `ACharacter` 是用于人形角色的高级类，带完整的行走、跳跃、动画系统。

#  53.`MakeEffectContext()`&`MakeOutgoingSpec()`

### 🔹 `MakeEffectContext()`

```cpp
FGameplayEffectContextHandle UAbilitySystemComponent::MakeEffectContext()
```

**作用：**
创建一个 `FGameplayEffectContextHandle`，它用来记录这次效果的来源信息，如：

* 是谁释放的技能
* 命中的是谁
* 是否是暴击
* 是否穿透
* 是近战还是远程……

👉 **用处：** 后续可以在属性变化函数中取出 `EffectContext` 来判断是谁对我造成了影响（例如造成伤害的敌人是谁）。

---

### 🔹 `MakeOutgoingSpec(...)`

```cpp
FGameplayEffectSpecHandle UAbilitySystemComponent::MakeOutgoingSpec(
    TSubclassOf<UGameplayEffect> GameplayEffectClass,
    float Level,
    FGameplayEffectContextHandle Context
)
```

**作用：**
根据指定的 `GameplayEffect` 类、等级和上下文，**构建一个即将被应用的效果规格（Spec）**。

* `GameplayEffectClass`：你要应用的 GE 类（如加生命、伤害）
* `Level`：效果的等级，会影响计算值
* `Context`：这个 GE 的上下文（谁发出的、是否暴击等）

👉 **用处：** 你可以对 `SpecHandle` 做进一步配置，然后再应用。

# 54.Derived Attributes
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250622155415593.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250622190301329.png)
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250622194103075.png)

# 55.int32

## ✅ 为什么使用 `int32`

`int32` 是 Unreal Engine（UE）推荐的标准整型类型，原因如下：

| 项目              | 原因                                                      |
| --------------- | ------------------------------------------------------- |
| ✅ **跨平台一致性**    | `int32` 在所有平台上都明确是 **32 位有符号整数**，而 `int` 在某些平台可能不是。     |
| ✅ **与UE宏系统兼容**  | UPROPERTY、RPC（如 `ReplicatedUsing`）等系统依赖确定大小的类型。         |
| ✅ **内存和网络效率**   | 32 位整数足够表示常见游戏中的等级、金钱、积分等，同时不会浪费太多内存。                   |
| ✅ **引擎源码中广泛使用** | 引擎本身大量使用 `int32`、`float`、`FString` 等统一类型，所以跟随规范有助于可维护性。 |

# 56.static
## ✅ 一句话总结

> `static` 的关键作用在于 **控制“作用域”和“生命周期”**：
> 类中用于**共享**，函数中用于**保留状态**，文件中用于**隐藏**。

---

## ✅ C++ 中 5 类常见 `static` 用法对比总结

| 类别            | 所属位置      | 作用范围        | 生命周期     | 常见用途        | 
| ------------- | --------- | ----------- | -------- | ----------- | 
| 1. 静态成员变量     | 类中        | 类作用域，所有对象共享 | 程序全程     | 所有对象共享数据，全局计数器、配置池等 | `static int Count;`         |
| 2. 静态成员函数     | 类中        | 类作用域，可类名调用  | 程序全程     | 工具类函数、访问类的静态状态 | `static void Init();`       |
| 3. 局部静态变量     | 函数中       | 函数局部        | 函数第一次调用后 | 缓存、延迟初始化、惰性单例 | `static int i = 0;`         |
| 4. 静态自由函数（类外） | .cpp 文件   | 当前源文件（编译单元） | 程序全程     | 不暴露实现细节（私有 .cpp 工具函数）   | `static void Helper();`     |
| 5. 静态全局变量     | .cpp 文件全局 | 当前源文件（编译单元） | 程序全程     | 限定全局变量作用域，防止冲突      | `static int GlobalVar = 5;` |

---


## ⚠️ 常见注意事项与误区

### 1️⃣ 静态成员变量要在类外定义一次
### 2️⃣ 静态成员函数不能访问非静态成员
### 3️⃣ 函数内部 static 变量只初始化一次
### 4️⃣ `.cpp` 文件中声明的 `static` 函数/变量无法跨文件访问
### 5️⃣ 静态变量不是线程安全的（默认）

* 函数内的静态变量，在多线程并发首次初始化时可能出现竞态问题（C++11 起大多已线程安全，但依编译器）
* 若涉及并发，建议使用 `std::call_once` 或其他线程同步机制

# 57.单例（静态单例标签注册器）
## ✅ 这个单例的设计目的

这是一个用于注册原生（Native）Gameplay Tags 的**全局工具类**，你不需要每次都创建 `FAuraGameplayTags` 实例，而是通过静态接口全局访问、注册与查询标签。

---

## ✅ 单例运行机制解析

### 🌟 核心代码逻辑：

```cpp
// 静态成员变量定义（必须定义一次）
FAuraGameplayTags FAuraGameplayTags::GameplayTags;

// 获取实例（静态函数，类名调用）
static const FAuraGameplayTags& Get() { return GameplayTags; }

// 初始化注册标签
void InitializeNativeGameplayTags() {
    UGameplayTagsManager::AddNativeGameplayTag(...);
}
```

### 💡 运行时流程：

1. **程序启动时不会立刻创建 `GameplayTags`**，但它是静态变量，生命周期是整个程序期间。
2. 当你第一次调用 `FAuraGameplayTags::Get()` 或 `InitializeNativeGameplayTags()` 时，它开始“活跃”。
3. `InitializeNativeGameplayTags()` 会调用 **`UGameplayTagsManager` 注册标签**，这些标签可用于 Ability System。
4. 通过 `FAuraGameplayTags::Get()`，你可以在任何地方访问这些标签，不用传对象、不用重复写字符串。

---

## ✅ 为什么 UE 的 GameplayTags 推荐用这种写法

| 原因             | 解释                                                  |
| -------------- | --------------------------------------------------- |
| ✅ 不重复注册标签      | 所有标签集中初始化一次                                         |
| ✅ 全局访问统一接口     | 任意 Actor、Ability 都能通过 `FAuraGameplayTags::Get()` 获取 |
| ✅ 避免字符串硬编码错误   | 比如 `"Attributes.Strenght"` 写错也不会崩                   |
| ✅ 编译时检查、智能提示支持 | 标签都在源码中注册，IDE 可提示和重构                                |
## 延迟构造的全局单例类
```cpp
class FMySingleton
{
public:
	static FMySingleton& Get()
	{
		static FMySingleton Instance; // ⚠️ C++11 保证线程安全的懒汉式构造
		return Instance;
	}
private:
	FMySingleton() {}
};
```

# 58.原生（Native）Gameplay Tags


**Native Gameplay Tags** 是指：

> 在 C++ 代码中，通过 `UGameplayTagsManager::AddNativeGameplayTag()` 显式注册的标签，而不是在 `.ini` 或 Editor 中创建的。拥有更好的安全性、结构化和自动化支持，是大型项目推荐的标签管理方式。

### ✅ 特点：

| 原生 Gameplay Tag                                    | 非原生 Gameplay Tag（也叫 Dynamic Tag）         |
| -------------------------------------------------- | ---------------------------------------- |
| 在 C++ 中注册（如 `InitializeNativeGameplayTags`）        | 在 Editor 或 `DefaultGameplayTags.ini` 中声明 |
| 编译期可见，支持智能提示、自动补全                                  | 字符串字面值，容易拼写错误                            |
| 支持结构化封装，如 `FAuraGameplayTags::Attributes_Strength` | 不方便管理、分类不清晰                              |
| 程序启动时注册，仅注册一次                                      | 可能来自动态资源或配置                              |

---

## ✅ 使用原生 Tag 的好处

1. ✅ 编译期安全 —— 避免 `"Effect.Buff.Strenght"` 这种拼错字符串
2. ✅ 支持结构化访问 —— 可集中封装到 `FAuraGameplayTags` 中
3. ✅ 高性能 —— 启动时注册一次，不依赖资源加载
4. ✅ 自动提示 / 重构友好 —— UE Editor 和 Rider 可识别
5. ✅ 跨团队标准化 —— 所有标签统一来源、文档清晰

# 59.FString vs FName vs FText

## ✅ 底层对比图（结构简化）

> `FString` 是灵活可编辑的字符串，`FName` 是高效唯一的名字标识符，`FText` 是支持多语言的本地化字符串。三者设计各自针对不同功能，底层结构、性能和用途都不同。

```cpp
FString
 └── TArray<TCHAR>
       └── 内存：独立存储字符串

FName
 └── Index: uint32 → NameEntryPool["PlayerName"]
 └── Number: 0/1/2 → 支持自动编号

FText
 └── TSharedRef<ITextData>
       └── FString 字符串内容
       └── Namespace + Key
       └── 语言信息 + 本地化数据
```

---

## ✅ 一表对比：FString vs FName vs FText（底层实现）

| 特性/类型  | `FString`             | `FName`                        | `FText`                                    |
| ------ | --------------------- | ------------------------------ | ------------------------------------------ |
| 底层结构   | `TArray<TCHAR>`（动态数组） | `uint32 Index + uint32 Number` | `TSharedRef<ITextData>` 包裹 `FString` + 元信息 |
| 是否可变   | ✅ 是                   | ❌ 否（内部常量池）                     | ⚠️ 否（不可直接修改，只能构造）                          |
| 内存分配   | 每个 `FString` 都会单独分配内存 | 所有字符串集中在 `FNameEntry` 池中共享     | 共享文本数据，延迟翻译，支持 GC                          |
| 快速比较性能 | ⚠️ 慢（字符逐个比对）          | ✅ 极快（比较索引）                     | ⚠️ 慢（需要结构比对）                               |
| 本地化支持  | ❌ 不支持                 | ❌ 不支持                          | ✅ 支持（多语言文本系统）                              |
| 底层用途   | 任意字符串数据               | 名字、标识符、枚举标签等                   | 面向用户的可翻译文本，如 UI、对白                         |

---

## 🔍 1. FString 底层实现细节

```cpp
class FString : public TArray<TCHAR>
```

### ✅ 本质：

* 是一个对 `TArray<TCHAR>` 的封装（即一个动态宽字符数组）。
* 内存是**每个字符串独立开辟的**。
* 完全支持修改、拼接、格式化、切割等操作。

### ⚠️ 缺点：

* 每次比较都需要逐字符进行，性能较低；
* 不支持多语言翻译。

---

## 🔍 2. FName 底层实现细节

```cpp
struct FName
{
	uint32 ComparisonIndex; // 字符串在表中的索引
	uint32 Number;          // 编号，支持“Name_1”, “Name_2”等
};
```

### ✅ 本质：

* 所有 FName 的字符串都存放在一个 **全局字符串池（Name Table）** 中，称为 `FNameEntry`.
* 每个 `FName` 只记录字符串的 **索引编号**，不是字符串本身！

### ✅ 优势：

* 比较两个 FName 只需比较整数索引，速度非常快；
* 避免重复存储字符串，节省内存。

### ⚠️ 缺点：

* 字符串只可读，不可变；
* 不支持翻译（不面向用户显示）。

这是 `FName` 的两个核心成员变量：

```cpp
uint32 ComparisonIndex; // 字符串的唯一索引（指向全局 FNameEntry 表）
uint32 Number;          // 同一个字符串的编号，用于区分 Name_1、Name_2 这样的情况
```


## ComparisonIndex&Number区别：

| 属性     | ComparisonIndex       | Number                   |
| ------ | --------------------- | ------------------------ |
| 作用     | 表示字符串在全局表中的 ID        | 表示此字符串的“第几个版本”           |
| 类型     | `uint32`，唯一、内部映射      | `uint32`，附加编号            |
| 构造值影响  | 改变字符串实际含义             | 改变 `"Name"` → `"Name_1"` |
| 是否参与比较 | ✅ 是，决定两个 `FName` 是否相等 | ✅ 是                      |

---

## 🔧 额外提示：

```cpp
FName Name("MyKey", 3);
FString RealName = Name.ToString(); // 会得到 "MyKey_3"
```


---

## 🔍 3. FText 底层实现细节

```cpp
class FText{ TSharedRef<ITextData> TextData;};
```

### ✅ 本质：

* 是一个对共享数据的封装，内部存储实际字符串 + 本地化标识 + 语言信息。
* 数据通过 `TSharedRef` 管理引用计数，支持 GC。
* 支持运行时根据语言环境切换文本（即动态翻译）。

### 🔧 ITextData 中可能包含：

* 原始字符串
* 源语言
* 当前语言版本
* 命名空间与键（用于查找翻译）

### ⚠️ 缺点：

* 构造复杂；
* 不能直接拼接或修改（只能重建）；
* 比较两个 `FText` 开销较大。

---

## ✅ 一览：常用 `FText` 相关函数

| 方法/宏                | 用途                 | 是否支持翻译 | 示例                                                                        |
| ------------------- | ------------------ | ------ | ------------------------------------------------------------------------- |
| `FText::FromString` | 从 FString 创建 FText | ❌ 否    | `FText::FromString("Hello")`                                              |
| `LOCTEXT`           | 静态本地化文本（类内宏）       | ✅ 是    | `LOCTEXT("Key", "你好")`                                                    |
| `NSLOCTEXT`         | 命名空间本地化文本（函数内）     | ✅ 是    | `NSLOCTEXT("UI", "Start", "开始游戏")`                                        |
| `FText::Format`     | 插入变量 / 句子模板拼接      | ✅ 是    | `FText::Format(NSLOCTEXT("UI", "Score", "分数：{0}"), FText::AsNumber(100))` |


---

### ✅ 1. `FText::FromString`（不支持翻译 ❌）

```cpp
FText Text = FText::FromString("Hello World");
```

> ⚠️ 用于日志、调试、非本地化 UI（建议仅在内部使用）

---

### ✅ 2. `LOCTEXT("Key", "内容")`（类内宏 ✅）

```cpp
#define LOCTEXT_NAMESPACE "MyClass"

FText Title = LOCTEXT("StartGameTitle", "开始游戏");

#undef LOCTEXT_NAMESPACE
```

* 宏定义的方式，使用当前文件/类名作为命名空间
* 适合 `.cpp` 文件内大段静态文本

---

### ✅ 3. `NSLOCTEXT("Namespace", "Key", "内容")`（函数内 ✅）

```cpp
FText Tip = NSLOCTEXT("UI", "ExitTip", "你确定要退出游戏吗？");
```

* 推荐用于函数体内
* 命名空间 `"UI"` 和 key `"ExitTip"` 用于后续翻译导出识别

---

### ✅ 4. `FText::Format`（格式化带变量 ✅）

> 类似 `printf`、`std::format`，但支持 `FText` 本地化！


```cpp
FText FormatText = FText::Format(
	NSLOCTEXT("UI", "PlayerStatus", "玩家 {0} 等级 {1}"),
	FText::FromString("Alice"),
	FText::AsNumber(10)
);
```

---


### ✅ 数值格式化辅助函数

| 函数                         | 示例输出     |
| -------------------------- | -------- |
| `FText::AsNumber(1234)`    | `1,234`  |
| `FText::AsPercent(0.75)`   | `75%`    |
| `FText::AsCurrency(99.99)` | `￥99.99` |

---

# 60.TEXT()


### ✅ `TEXT()` 是什么？

* `TEXT("内容")` 是一个宏，用来生成**宽字符字符串（TCHAR\*）**
* 例如：`TEXT("你好")` → `L"你好"`（Windows 平台）

---

### ✅ 为什么要用 `TEXT()`？

| 原因            | 说明                                       |
| ------------- | ---------------------------------------- |
| 跨平台           | 自动适配不同平台的字符类型                            |
| 支持中文          | 宽字符支持 Unicode（如中文）                       |
| 与 Unreal 类型兼容 | `FString`、`FText`、`UE_LOG` 等都要求 `TCHAR*` |

---

### ✅ 正确用法示例：

```cpp
FString Str = TEXT("你好");
FText Txt = FText::FromString(TEXT("欢迎"));
UE_LOG(LogTemp, Log, TEXT("调试信息"));
```
---

### ✅ 记忆口诀：

> 所有字符串，加上 `TEXT()`，中文无忧，平台通吃！

# 61.Outer
## ✅ `Outer` 是什么？

在 Unreal Engine 中，`Outer` 是 UObject 系统的一个**所有权概念**，表示：

> **当前对象的“外部拥有者”或“容器对象”**，用于生命周期、垃圾回收（GC）和路径层级管理。

---

## ✅ 这行代码中的 `this` 是谁？

```cpp
AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(
    this, AttributeMenuWidgetControllerClass);
```

* `this` 是 **该类**`AAuraHUD` 实例指针
* 所以这里表示：`AAuraHUD` 是你创建的 `UAttributeMenuWidgetController` 的 Outer（所有者），当你 HUD 被销毁（比如离开地图），菜单控制器也会一起销毁。

---

## ✅ 为什么要传 Outer？

* **Unreal 引擎中的对象树结构**：对象路径会嵌套，如：

  ```
  /Game/Maps/Map1.Map1:PersistentLevel.AuraHUD_0.AttributeMenuWidgetController_0
  ```

* 设置 `this` 为 Outer 有以下好处：

| 功能       | 说明                                    |
| -------- | ------------------------------------- |
| ✅ 垃圾回收控制 | `Outer` 被销毁时，`NewObject` 创建的子对象也会被销毁  |
| ✅ 路径自动命名 | 引擎会自动为新对象命名：`OuterName_ObjectName_0`  |
| ✅ 生命周期绑定 | 避免你手动去 `New` 和 `delete`，由 Unreal 管理内存 |
| ✅ 避免对象泄漏 | 如果 `Outer` 没有 GC 引用链，相关子对象也会一并释放      |


## ✅ 小结

| 参数名     | 解释        | 建议值                                               |
| ------- | --------- | ------------------------------------------------- |
| `Outer` | 新对象的“拥有者” | 一般传 `this`                                        |
| `Class` | 要创建的类     | 如 `UAttributeMenuWidgetController::StaticClass()` |

```cpp
NewObject<ObjType>(Outer, Class);
```
# 62.委托签名强制匹配
在您提供的代码中，虽然 `Data` 参数在 lambda 表达式内部没有被直接使用，但它必须被声明为参数的原因与 Unreal Engine 的委托系统设计密切相关。以下是详细解释：

---

### **1. 委托签名强制匹配**
- **`FOnAttributeChangeData` 是委托的固定签名**  
  Unreal 的 `GetGameplayAttributeValueChangeDelegate()` 返回的委托类型明确定义为：
  ```cpp
  DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttributeChangeDelegate, const FOnAttributeChangeData&);
  ```
  - 即使您不需要 `Data` 参数，lambda 也必须接受该参数以匹配委托签名
  - **类比**：就像实现接口时必须包含所有方法，即使某些方法为空

---

### **2. 潜在的使用场景**
虽然当前代码未使用 `Data`，但保留它有重要价值：
- **未来扩展性**：  
  后续可能需要访问 `Data.NewValue`/`Data.OldValue` 做差值计算
- **调试需求**：  
  可通过 `Data.Attribute` 验证触发的是哪个属性变化
- **引擎内部机制**：  
  部分 UE 内部逻辑可能依赖参数传递（如序列化）

---

### **3. 代码优化建议**
如果确定不需要 `Data`，可以通过以下方式明确标记：
```cpp
.AddLambda([this, Pair, AS](const FOnAttributeChangeData& /*Data*/)  // 注释参数名
{
    // ...
}
```
或使用 C++17 的 `[[maybe_unused]]`：
```cpp
.AddLambda([this, Pair, AS]([[maybe_unused]] const FOnAttributeChangeData& Data)
{
    // ...
}
```

---

### **4. 为什么不能省略参数？**
- **类型安全**：UE 的委托系统在编译时会严格检查参数数量和类型
- **二进制兼容性**：省略参数会导致函数指针的调用约定不匹配
- **多播委托机制**：底层需要统一的参数传递栈空间

---

### **5. 您代码的实际需求**
您的逻辑是通过 `Pair.Value()` 直接获取属性值，而非依赖 `Data` 的新旧值。这种写法适用于：
- 当只需要属性当前值，不关心变化量时
- 当多个属性共享同一处理逻辑时

但要注意：**如果 `AS` 是局部变量或可能失效的指针，此写法有风险**。更安全的做法是：
```cpp
// 在类成员中存储AttributeSet的弱引用
TWeakObjectPtr<const UAttributeSet> WeakAS = AS;

.AddLambda([this, Pair, WeakAS](const FOnAttributeChangeData&)
{
    if (const UAttributeSet* TargetAS = WeakAS.Get())
    {
        Info.AttributeValue = Pair.Value().GetNumericValue(TargetAS);
        // ...
    }
}
```

---

### **总结**
| 现象 | 原因 | 解决方案 |
|------|------|----------|
| 必须声明未使用的 `Data` 参数 | UE委托系统强制签名匹配 | 保留参数但标记为未使用 |
| 直接通过 `Pair.Value()` 取值 | 避免重复查询属性 | 确保 `AS` 指针安全 |
| 广播 `AttributeInfoDelegate` | 通知UI更新 | 检查接收方是否正确绑定 |

这种设计体现了 UE 委托系统在灵活性和类型安全之间的权衡，虽然会引入看似"无用"的参数，但能保证系统的健壮性。

# 64.GameplayAbility中不要使用的选项
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250630144246286.png)

# 65.为什么模板函数定义写在头文件？

```cpp
template <class UserClass, typename PressedFuncType, typename ReleaseFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(...) {
    check(InputConfig);
}
```

### ✅ 这是**模板函数的定义必须写在头文件里**，否则链接时报错

在 C++ 中：

* 模板函数只有在 **知道所有模板参数的情况下** 才能生成代码（即实例化）
* 如果你只在 `.cpp` 文件中定义了模板函数，编译器在别的文件实例化它时 **找不到实现**，就会链接错误

### 所以必须这样做：

* 模板函数声明和定义**全部放在 `.h` 文件中**
* 这样每次实例化时，编译器都能“看到”完整函数实现

### ⚠️ 这就造成了模板函数**行为上就是内联的**（不是你写了 `inline` 关键字，而是编译行为决定）

> 💡 **你没有显式写 `inline`，但模板函数由于定义在头文件中，本质上就是 inline 的**

---

# 66.`class` vs `typename` 的区别？

### ✅ 完全等价！

## ✅ 总结表格

| 特性     | `template<class T>` | `template<typename T>` | 区别              |
| ------ | ------------------- | ---------------------- | --------------- |
| 含义     | 模板类型参数              | 模板类型参数                 | ❌ 无区别（除嵌套类型上下文） |
| 推荐写法   | 可读性略低               | ✅ 推荐                   | ✔️              |
| 用于嵌套类型 | ❌ 不允许               | ✅ 必须使用 `typename`      | ✔️              |



## ✅ 模板函数定义位置总结

| 写法         | 是否可行 | 说明                         |
| ---------- | ---- | -------------------------- |
| `.h` 文件中定义 | ✅ 必须 | 模板实例化时编译器需要看到实现            |
| `.cpp` 中定义 | ❌ 不行 | 链接时报错（undefined reference） |

# 67.为什么特别是指针要用 `TObjectPtr + UPROPERTY`

## ✅ 为什么特别是指针要用 `TObjectPtr + UPROPERTY`

UE5 引入 `TObjectPtr<T>` 是为了更好地支持 GC 安全，但它仍然必须搭配 `UPROPERTY` 才能真正被引擎管理。

```cpp
TObjectPtr<UAuraInputConfig> InputConfig; // ❌ GC 不会追踪这个
UPROPERTY() TObjectPtr<UAuraInputConfig> InputConfig; // ✅ 会追踪这个
```

# 68.ThisClass

## ✅ `ThisClass` 是什么？

在 Unreal Engine 的 C++ 宏体系中，`ThisClass` 是由 `GENERATED_BODY()` 宏自动生成的一个 **类型别名**，等价于当前类的名字。


```cpp
&ThisClass::AbilityInputTagReleased
其实完全等价于：
&AAuraPlayerController::AbilityInputTagReleased
```

---

## ✅ 那为什么要用 `ThisClass`？

这是一种 UE 推荐的写法，有以下几个好处：

| 优点         | 解释                                          |
| ---------- | ------------------------------------------- |
| ✅ 更通用      | 如果你将来复制粘贴这段代码到另一个类，不用手动改类名                  |
| ✅ 避免写错类名   | IDE 自动识别 `ThisClass`，不易出错                   |
| ✅ 和 UE 宏兼容 | `GENERATED_BODY()` 内部就用 `ThisClass` 表示当前类类型 |
| ✅ 写法一致     | 和 `Super::` 类似，是 UE C++ 的标准风格之一             |

# 69.函数指针

### 📌 本质：

函数指针是 **一个变量，保存着函数的内存地址**。通过它，可以“**像数据一样传递函数**”。


### 🎯 目的：**“函数作为参数、变量或返回值传递”**

这是让程序拥有**灵活行为切换能力**的基础。因为：

| 没有函数指针时 | 你只能写死逻辑，运行期不能改变行为 |
| 有了函数指针   | 函数变成“数据”，可以动态传入、替换、组合 |



### 1️⃣ 回调机制（Callback）

当某个函数执行完后，调用另一个你传进去的函数：

```cpp
void OnFinish() {
    std::cout << "Done!\n";
}
void DoSomething(void (*Callback)()) {
    Callback();  // 回调
}
DoSomething(&OnFinish);
```

✅ 实际用例：Unreal 的 `Delegate`、C 的 `qsort()`、线程完成回调

---

### 2️⃣ 行为选择器（策略模式）

用函数指针作为参数来切换算法：

```cpp
int Add(int a, int b) { return a + b; }
int Mul(int a, int b) { return a * b; }

int Calc(int x, int y, int (*op)(int, int)) {
    return op(x, y);
}
Calc(3, 4, &Add);  // 7
Calc(3, 4, &Mul);  // 12
```

✅ 实际用例：AI 决策、角色攻击策略切换、材质动态处理函数等

---

### 3️⃣ 事件处理系统（比如输入、UI）

绑定成员函数指针作为输入响应：

```cpp
BindAction(SomeAction, ETriggerEvent::Started, this, &MyClass::OnFire);
```

✅ Unreal 中的 `BindAction()` 就是函数指针 + 对象组合实现

---

### 4️⃣ 定时器 / 异步任务调度

```cpp
GetWorldTimerManager().SetTimer(MyHandle, this, &AMyActor::FireWeapon, 1.0f, true);
```

* 调用传入的函数地址（而不是硬编码哪个函数）

---

### 5️⃣ 插件系统 / 动态行为加载

函数指针让你可以在**运行期加载库/脚本并执行函数**（比如 `dlsym()` 或 Lua 脚本）


## ✅ Bonus：更现代的选择是什么？

在现代 C++（和 Unreal）中，为了更安全灵活，我们会更常用：

| 替代品                           | 说明                                  |
| ----------------------------- | ----------------------------------- |
| `std::function` / `TFunction` | 可以接受任意函数、Lambda、成员函数等               |
| `Delegate`                    | Unreal 封装的函数回调机制，支持反射与 UObject 生命周期 |
| `Lambda`                      | 可以内联定义行为、捕获变量，比函数指针更灵活              |



---

非常好！这是 C++ 进阶但非常重要的知识点 —— **普通函数指针** vs **成员函数指针**，尤其在 Unreal Engine 这种强面向对象的引擎中用得非常多，比如输入系统、委托（Delegate）、Timer、任务系统、回调函数等。

---

## ✅ 普通函数指针 vs 成员函数指针 —— 全面对比表

| 类别       | 类型写法                   | 示例                   | 调用方式                 | 是否依赖对象？ | 典型用途                   |
| -------- | ---------------------- | -------------------- | -------------------- | ------- | ---------------------- |
| ✅ 普通函数指针 | `void (*Func)()`       | `&GlobalFunction`    | `Func();`            | ❌ 否     | 全局函数、静态函数、C 风格回调       |
| ✅ 成员函数指针 | `void (Class::*)(...)` | `&MyClass::MyMethod` | `(Object->*Func)();` | ✅ 是     | 类方法调用、Unreal 输入绑定、事件系统 |

---

## 🔍 一、普通函数指针

### ✅ 定义

```cpp
void Hello() {
    std::cout << "Hello!" << std::endl;
}

void (*FuncPtr)() = &Hello;
FuncPtr();  // 调用
```

* 函数地址保存在变量 `FuncPtr` 中
* 它不需要对象实例，**可以直接调用**



---

## 🔍 二、成员函数指针

### ✅ 定义

```cpp
class MyClass {
public:
    void Greet() {
        std::cout << "Hello from class!" << std::endl;
    }
};

MyClass Obj;

void (MyClass::*MethodPtr)() = &MyClass::Greet;
(Obj.*MethodPtr)();  // 调用成员函数
```

* `MethodPtr` 存的是类成员函数地址
* 不能直接调用，**必须配合对象实例使用：`obj->*funcptr()` 或 `obj.*funcptr()`**

---

## ⚠️ 为什么成员函数指针需要对象？

因为成员函数 **隐式地依赖 `this` 指针**，你不能像调用普通函数那样只传参数，它还得知道是哪一个对象调用的。

---

## ✅ 扩展：静态成员函数属于哪类？

```cpp
class MyClass {
public:
    static void StaticFunc();
};
```

> ⚠️ **静态成员函数本质上是普通函数！**

所以它的指针是：

```cpp
void (*StaticPtr)() = &MyClass::StaticFunc;
```

你不需要对象也能调用它。

---


## ✅ Bonus：Unreal 的 Delegate 是如何支持成员函数回调的？

Unreal 内部其实使用了一个复杂的封装系统（如 `TBaseDelegate`, `TFunction`, `FDelegateHandle` 等）来保存：

* 对象指针
* 函数指针（成员函数指针）
* 参数签名

> 所以你可以写：

```cpp
Delegate.AddUObject(this, &MyClass::OnEvent);
```

Unreal 会自动管理 `this` 和 `函数地址` 的配对关系。


---



### ✅ 总结知识点

| 指针类型       | 类型名                    | 是否依赖对象 | 调用方式                   | 典型用途 |
| ---------- | ---------------------- | ------ | ---------------------- | ------- |
| 普通函数指针     | `void (*)()`           | 否      | `Func()` 或 `(*Func)()` | 普通函数回调、C 接口        |
| 成员函数指针     | `void (Class::*)()`    | ✅ 是    | `(Object->*Func)()`    | 类方法回调、Input 系统       |
| 静态函数指针     | `static void (*)()`    | 否      | 和普通函数一样                | ✅       |
| Lambda 表达式 | `[this](){}` 或 `[&]{}` | 可选     | 立即调用或延迟封装              | 动态任务、Delegate、Timer       |

# 70.循环变量的捕获注意事项

### 🧠 结论（记忆口诀）：

> **“引用捕获循环变量，等于踩雷引爆炸；值捕获才安全，延迟执行才靠谱。”**

---

如果你想看一张图表示“引用捕获”共享地址 vs “值捕获”复制变量，也可以告诉我，我来帮你画出。


### ✅ 你的原始写法（正确）：

```cpp
for (auto& Pair : AS->TagsToAttributesMap)
{
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
        [this, Pair](const FOnAttributeChangeData& Data)
        {
            BroadcastAttributeInfo(Pair.Key, Pair.Value());
        }
    );
}
```

在这个循环中，你 **正确地按值捕获了 `Pair`**，意思是：每次循环都将当前的 `Pair` 拷贝进 lambda，一共创建了多个 lambda，每个 lambda 都独立、安全地持有自己那一对 `Key-Value`。

---

### ❌ 假设你错误地写成 `[this, &Pair]`：

```cpp
for (auto& Pair : AS->TagsToAttributesMap)
{
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
        [this, &Pair](const FOnAttributeChangeData& Data)
        {
            BroadcastAttributeInfo(Pair.Key, Pair.Value());
        }
    );
}
```

### 🚨 会发生什么问题？

1. 你捕获的是 **循环变量 Pair 的引用**，注意：这个变量 **在循环中只存在一份（内存地址不变）**。
2. 所有 lambda 捕获的其实是同一个 `Pair` 的地址。
3. 当循环跑完，`Pair` 作用域结束，它指向的值就变成了**最后一对键值对**，但 lambda 还存着引用地址。
4. **结果：所有 lambda 的 `Pair` 都指向同一个值，不正确；甚至有崩溃风险**。



# 71.**批量绑定技能输入回调函数**（按键绑定技能）


```cpp
template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void BindAbilityActions(...)
```

这是一个模板函数，支持绑定不同签名的回调方法。

---

## 🧩 函数参数解释

```cpp
BindAbilityActions(
    const UAuraInputConfig* InputConfig,  // 数据资源，定义了输入动作与标签的对应关系
    UserClass* Object,                    // 一般是 PlayerController 或 Character
    PressedFuncType PressedFunc,          // 按下回调函数指针
    ReleasedFuncType ReleasedFunc,        // 松开回调函数指针
    HeldFuncType HeldFunc                 // 持续触发回调函数指针
)
```

* `InputConfig` 是一个 Blueprint DataAsset，里面存储了每个技能的输入动作（InputAction）和标签（GameplayTag）。
* `Object` 是你希望在哪个对象（类）上执行这些函数（比如 PlayerController）。
* 后面三个函数指针是你希望绑定给按键按下、松开、长按事件的处理函数。

## ✅ 总结：你这段代码的作用是——

| 内容                            | 作用                |
| ----------------------------- | ----------------- |
| **模板函数 `BindAbilityActions`** | 支持绑定不同类、不同函数类型的回调 |
| **自动遍历配置文件 InputConfig**      | 不用手动硬编码每个技能的按键绑定  |
| **支持按下 / 松开 / 长按三种触发方式**      | 灵活适配不同技能需求        |
| **使用 `InputTag` 传入函数**        | 使函数中可以区分哪个技能被触发   |

---

## ✅ 一个实际调用例子（在 PlayerController 中）：

```cpp
AuraInputComponent->BindAbilityActions(InputConfig, this,
    &AAuraPlayerController::AbilityInputTagPressed,
    &AAuraPlayerController::AbilityInputTagReleased,
    &AAuraPlayerController::AbilityInputTagHeld);
```

然后你只需要在 `AAuraPlayerController` 类中写这三个函数接收触发：

```cpp
void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag) { ... }
void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag) { ... }
void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag) { ... }
```

# 72.可变参模板函数
### 🧠 为什么是可变参函数？

因为你要传额外的参数，比如：

```cpp
BindAction(InputAction, ETriggerEvent::Started, this, &Func, InputTag);
```

这个 `InputTag` 并不是默认的参数，而是你传给回调函数的**附加上下文**，所以必须通过模板 + 可变参数支持。

---


### ✅ 对应的函数签名（可变参模板）可能是：

```cpp
template< class DelegateType, class UserClass, typename... VarTypes >
FInputActionBinding& BindAction(
    const FName ActionName,
    const EInputEvent KeyEvent,
    UserClass* Object,
    typename DelegateType::template TMethodPtr< UserClass > Func,
    VarTypes... Vars
);
```

或在 Enhanced Input 中类似的版本：

```cpp
template <class UserClass, typename FuncType, typename... ParamTypes>
FEnhancedInputActionEventBinding& BindAction(
    const UInputAction* Action,
    ETriggerEvent TriggerEvent,
    UserClass* Object,
    FuncType Func,
    ParamTypes&&... Params
);
```

这就是**可变参函数**，特点是：

* `... ParamTypes`：可以接受任意个参数。
* `Params&&...`：用右值引用转发（完美转发），支持传入值、引用、const引用等。

# 73.右值引用转发（完美转发）
这个问题非常重要，是理解 **现代 C++（C++11 之后）** 的核心概念之一。



## ✅ 一、什么是右值引用（`T&&`）？

### 📌 左值 vs 右值

* **左值（lvalue）**：有名字、能被取地址的对象。
  例：`int x = 5;` 中的 `x` 是左值。

* **右值（rvalue）**：临时对象、字面量，不能取地址。
  例：`5`、`x + 1` 是右值。

---

### ✅ C++11 引入：右值引用 `T&&`

```cpp
void Take(int&& x);  // 接收一个右值引用
Take(5);             // ✔ 合法
Take(x);             // ❌ 错误，x 是左值
```

**右值引用的作用：**
允许你“捕获临时对象”，用于**资源复用**，避免复制（如移动构造函数、移动赋值）。

---

## ✅ 二、什么是完美转发（Perfect Forwarding）？

当你写一个模板函数时，你可能想把参数**原封不动地传递**给另一个函数（比如构造函数、回调等）。这时，如果你用错参数类型，可能就“损失语义”。

---

### ❌ 错误示例（用 `T`）：

```cpp
template<typename T>
void Wrapper(T arg) {
    Callee(arg);  // ❌ 这里把右值变成左值了
}
```

如果你这样调用：

```cpp
Wrapper(5);  // T= int, arg 是 int 类型 → 变成了左值 → 无法转发右值语义
```

---

### ✅ 正确示例（完美转发）：

```cpp
template<typename T>
void Wrapper(T&& arg) {
    Callee(std::forward<T>(arg));  // ✅ 保留原始语义（左值/右值）
}
```

**解释：**

| 元素                     | 含义                         |
| ---------------------- | -------------------------- |
| `T&&`                  | 是万能引用（Universal Reference） |
| `std::forward<T>(arg)` | 根据 T 的推导结果选择左值或右值传递        |
| `Wrapper(5)`           | T = int，右值 → 完美转发为右值       |
| `Wrapper(x)`           | T = int&，左值 → 完美转发为左值      |

---

## ✅ 举个直观例子

```cpp
void Print(int& x) { std::cout << "LValue\n"; }
void Print(int&& x) { std::cout << "RValue\n"; }

template<typename T>
void ForwardTest(T&& val) {
    Print(std::forward<T>(val));  // ✔ 保留语义
}
```

调用：

```cpp
int a = 10;
ForwardTest(a);    // 输出：LValue
ForwardTest(20);   // 输出：RValue
```

---

## ✅ 总结对比表

| 概念                   | 示例            | 用途与意义                |
| -------------------- | ------------- | -------------------- |
| 右值引用 `T&&`           | `int&& x`     | 捕获右值临时对象，支持移动语义      |
| 万能引用                 | `T&& val`（模板） | 可接收左值/右值，根据传入类型推导    |
| `std::forward<T>(x)` |               | 保留左值或右值的语义，实现完美转发    |
| 完美转发                 | 模板中转调用另一个函数   | 减少拷贝，提高效率，保留传入值的类型特征 |












