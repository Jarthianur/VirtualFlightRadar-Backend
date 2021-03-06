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
    - [Pull Requests](#pull-requests)
    - [Codestyle](#codestyle)
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

## Pull Requests

A PR may contain any change to the code, documentation, or whatever. We have a [template](pull_request_template.md) for that purpose, which should make it easier for You to create and us to process the PR. Every PR **must** build passing and be reviewed by, at least, one of the [authors](AUTHORS.md), responsible for that subject. Especially changes to the code should be reviewed and approved by people, who were involved in code development before. The coverage ratio must not decrease, which means, adding code includes unit tests. We use a custom framework, given as submodule, for this purpose. We also have [style](#codestyle) and [doc](#documentation) guidelines for code, which **should be followed, as far as possible**. Merging a PR may take some time, as we are testing this piece of Software really conscientious. This includes testing the system live, what might not always be immediately possible.

## Codestyle

We have some simple, but effective guidelines, on how the code should look like.

+ The code is written in C++14, using [boost](http://www.boost.org/) and STL
+ The code design is completely object oriented
+ Namings are short, but expressive
+ Names are in lowerCamelCase, enums and macros in UPPERCASE
+ Implementation files have *.cpp* extension
+ Header files have *.h* extension
+ Header files with inlined code have *.hpp* extension
+ Main code is placed in */src/*, tests in */test/*
+ Sourcefiles follow a package structure, with according namespaces
+ Class variables are prefixed with some metadata (prefix_varName):
  + m => member
  + s => static
+ Non-static members are not public, use getters/setters to access
+ Prefer std types over builtin types (e.g. std::int32_t instead of int)
+ Functions explicitely guaranteeing not to throw exceptions are marked with *noexcept*
+ Buffer accessing functions, without range check, are not permitted, instead use their safe candidates (sprintf -> snprintf)
+ Smart pointers are preferred over raw pointers

## Documentation

The project documentation should be easy to understand, sometimes less words mean more.
The code documentation should follow some guidelines listed here.

+ Every sourcefile begins with the copyright license
+ Descriptions are short, but expressive
+ The code documentation is based on Doxygen
+ Class and function descriptions should not contain implementation details
+ See [Doxygen](http://www.stack.nl/~dimitri/doxygen/manual/commands.html) for a command list
+ Additionally add `@threadsafe` if a function appears in a multithreaded context and is safe to use
