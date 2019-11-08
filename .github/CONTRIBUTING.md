# Contributing to VFR-B

It seems, that You're likely going to contribute to this repository. **Thanks for dealing with this project and all the effort You may invest in it.**
But to handle Your request as effective as possible, there are some guidelines we want You to follow.

## Contents

- [Contributing to VFR-B](#contributing-to-vfr-b)
  - [Contents](#contents)
  - [I just have a question](#i-just-have-a-question)
  - [Opening Issues](#opening-issues)
    - [What to issue, and how?](#what-to-issue-and-how)
    - [Feature requests](#feature-requests)
    - [Bug reports](#bug-reports)
  - [Pull requests](#pull-requests)
  - [Codestyle](#codestyle)
    - [General guidelines](#general-guidelines)
    - [Files and directories](#files-and-directories)
    - [Naming rules](#naming-rules)
    - [Code guidelines](#code-guidelines)
  - [Documentation](#documentation)

## I just have a question

You don't have to read this whole document, if You just have a question. There is even not always the need to open an issue for it. Just consider the following: *Is my question relevant to others?* If so, please open an [issue](ISSUE_TEMPLATE/question.md) and read the [Opening Issues](#opening-issues) section of this document. If not, we would recommend to send an email to the [authors](AUTHORS.md) (at least to the repo owner). We can than decide to make an issue out of it anyway. Either way, not to skimp on details about the subject may decrease the communication overhead and prevent questions from our side back to You. We give our best to respond ASAP.

## Opening Issues

### What to issue, and how?

An Issue can be opened for nearly everything, like bug reports, PRs, feature requests and so on.
To make it easier for You to create and us to process the Issue, we have issue templates for each purpose. Please follow these templates, or we might not be able to process it, if its subject is not clear to us. Additionally we recommend to label the Issue with according ones, to make the subject even more clear. Every request must comply to our [code of conduct](CODE_OF_CONDUCT.md). DON'T PANIC, all that sounds worse than it actually is.

### Feature requests

Also simply fill out the according points in the [Issue template](ISSUE_TEMPLATE/feature_request.md). Please provide detailed information, on what's its purpose and its value to You. This project grows by the needs of its users, not only by our decisions. So a feature relevant to You, might be relevant to others as well.

### Bug reports

Also simply fill out the according points in the [Issue template](ISSUE_TEMPLATE/bug_report.md). Please provide detailed information, on how to reproduce and its appearance. Reporting bugs, or weird behaviours, is essential to this project. Its correctness is much more important to us, than any feature. Exhaustive testing is not possible, even we give our best though, we might not catch all possible bugs and depend on every users findings.

## Pull requests

A PR may contain any change to the code, documentation, or whatever. We have a [template](pull_request_template.md) for that purpose, which should make it easier for You to create and us to process the PR. Every PR **must** build passing and be reviewed by, at least, one of the [authors](AUTHORS.md), responsible for that subject. Especially changes to the code should be reviewed and approved by people, who were involved in code development before. The coverage ratio must not decrease, which means, adding code includes unit tests. We use a custom framework, given as submodule, for this purpose. We also have [style](#codestyle) and [doc](#documentation) guidelines for code, which **should be followed, as far as possible**. Merging a PR may take some time, as we are testing this piece of Software really conscientious. This includes testing the system live, what might not always be immediately possible.

## Codestyle

We have some simple, but effective guidelines, on how the code should look like.

### General guidelines

+ Code is written purely in C++17, using [boost](http://www.boost.org/) and the standard library.
+ Code design is preferably object oriented.
+ Namings are short, but expressive.
+ Try to achieve high coherence and low coupling.
+ Think of single responsibility designs for objects.

### Files and directories

+ Source files have `.cpp` extension.
+ Header files have `.h` extension.
+ Header files with inlined code have `.hpp` extension.
+ Source files are placed in the `src/` directory.
+ Header files are placed in the `include/` directory.
+ Test files are placed in the `test/` directory.
+ All files follow a package structure, with according namespaces.
+ Files mainly containing a class declaration/definition are named like the class itself.
+ Files mainly containing function declarations/definitions are named in a meaningful way in `snake_case`.
+ Source and include directories are named in a meaningful way in `snake_case`.

### Naming rules

+ If not further specified, everything that is publicly exposed is named in `PascalCase`.
+ If not further specified, everything that is private/protected is named in `camelCase`.
+ All naming rules may be ignored, if required for compatibility.
+ Constants, macros and enum entries are named in all `UPPER_CASE`.
+ Namespaces are named in `snake_case`.
+ Primitive types and their aliases are named in `snake_case`.
+ Objects and their aliases are named in `PascalCase`.
+ Member fields are prefixed by `m_`
+ Static fields are prefixed by `s_` (private/protected), `S_` (public)
+ Function arguments are suffixed by `_`.

### Code guidelines

+ Use the primitive types provided in `include/types.h`.
+ Prefer classes over structs.
+ Member fields are generally not public and accessed by getter/setter functions.
+ Avoid setters where possible, rather use constructors and interfaces.
+ Recordtype structs are allowed with all public members.
+ Use `const` wherever applicable.
+ Do not use plain mutexes for locking, always wrap them in RAII guards.
+ Do not use unsafe functions, like buffer accessing without range checks.
+ Prefer smart pointers over raw pointers.
+ Avoid static fields, unless there is a good reason. Where applicable, combine with `thread_local`.
+ Use `#pragma once` instead of include guards.
+ Variable attributes, like `const`, are placed on the righthand side.

## Documentation

The project documentation should be easy to understand, sometimes less words mean more.
The code documentation should follow some guidelines listed here.

+ Every code containing file begins with the copyright license.
+ Descriptions are short, but expressive.
+ The code documentation is based on Doxygen.
+ Class and function descriptions should not contain implementation details.
+ See [Doxygen](http://www.stack.nl/~dimitri/doxygen/manual/commands.html) for a command list.
