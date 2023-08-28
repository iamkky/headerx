# headerx - A header extractor for mixed header + c code files

## Reasoning 

While there are compelling reasons to keep headers and C code in separate files — especially when a single header encompasses definitions from multiple C sources — certain scenarios would benefit from more integrated approach. In some cases, the header is so closely tied to its C source file that it feels like an introductory segment of that source. For these situations, the header could be well placed directly within the corresponding C source file.

Why not take advantage of this closeness? We could embed the header within its C source file and then utilize build tools to extract it during compile time for external purposes. This way, the project retains a single, unified file that houses both the header and the C code. The advantages are clear: fewer files to manage and a seamless integration of declarations and definitions.

This approach is particularly apt when an object-oriented philosophy is applied to a C project. It's not uncommon in such projects to have a "one class per file" setup, or perhaps a small cluster of related classes within a single file. For these scenarios, a blended header and source approach could be beneficial.

While I recognize this approach might fire up a passionate debates, sparking such discussions isn't the intent behind this modest software piece. It's just an experience!

## Usage

headerx extracts a header file embedded into a c source code. The use is as simples an extra command in the building process

```bash
header file.c
```

in file.c, header must be embedded in between specific comments.

use `\\HEADERX(<header filename>,<header tag>)` to indicate the staring of the header

and `\\ENDX` to end the header

Header file will be extracted to `<header filename>`, and will be protect by define tag `<header tag>` in the form of:

```C
#ifndef <header tag>
#define <header tag>

	Header body
	
#endif
```

## Example of mixed header + c code file

```C
//%%HEADERX(../awtk/HComponent.h, _AWTK_HCOMPONENT_H_)
#include <abd/new.h>
#include <helium/He.h>

Class(HComponent) {
	void		(*add)(HComponent, HComponent);
	He		(*render)(HComponent);
	HComponent	child;
	HComponent	last;
	HComponent	next;
};

Constructor(HComponent);
Destructor(HComponent);

void hcomponentAddChild(HComponent self, HComponent child);

He hcomponentRender(HComponent self);

//%%HEADERX

Constructor(HComponent)
{
	CInit(HComponent);
	
	self->render = hcomponentRender;
	self->add = hcomponentAddChild;
	self->child = NULL;
	self->next = NULL;
	self->last = NULL;

	return self;
}

Destructor(HComponent)
{
HComponent ch, ch_next;

	if(!self) return;

	for(ch=self->child; ch!=NULL; ch=ch_next){
		ch_next = ch->next;
		CFree(HComponent, ch);
	}

	free(self);
}

void hcomponentAddChild(HComponent self, HComponent child)
{
	if(nullAssert(self)) 	return;
	if(nullAssert(child))	return;

	if(self->child==NULL){
		self->child = child;
		self->last  = child;
	}else{
		self->last->next = child;
		self->last       = child;
	}
}

He hcomponentRender(HComponent self)
{
	return heNew("HComponent", heText("HComponent"), NULL);
}


```

## Notes

this file is part of AWTK project, and is here just as an example.

`Class`, `Constructor`, `Destructor`, `CInit` and `CFree` are defined in abd/new.h

