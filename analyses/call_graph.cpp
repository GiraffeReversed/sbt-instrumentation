#include "call_graph.hpp"
#include <llvm/IR/Function.h>
#include <list>
#include <iostream> 

#if LLVM_VERSION_MAJOR >= 4 || (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 5)
#include "llvm/IR/InstIterator.h"
#else
#include "llvm/Support/InstIterator.h"
#endif

using namespace llvm;
using dg::analysis::pta::PSNode;

const int RESERVE_SIZE = 4;

CGNode::CGNode(const Function* callerF, int nodeId) {
	caller = callerF;
	id = nodeId;
	//calls = std::vector<int>();
	calls.reserve(RESERVE_SIZE);
}

const Function* CGNode::getCaller() const {
	return caller;
}

int CGNode::getId() const {
	return id;
}

bool CGNode::containsCall(std::vector<CGNode> nodeMapping, const Function* call) {
	for(auto& node : calls) {
		if(nodeMapping[node].getCaller() == call) {
			return true;
		}
	}
	return false;
}

CallGraph::CallGraph(Module &M, std::unique_ptr<dg::LLVMPointerAnalysis> &PTA) {
	lastId = 0;
	nodes = std::vector<CGNode>();
	int distance = std::distance(M.begin(),M.end());
	nodes.reserve(distance+1);

	// Creates nodes from functions
	for (Module::iterator Fiterator = M.begin(), E = M.end(); Fiterator != E; ++Fiterator) {
		CGNode newNode(&*Fiterator, nodes.size());
		nodes.push_back(newNode);
	}

	// Go through functions
	for (Module::iterator Fiterator = M.begin(), E = M.end(); Fiterator != E; ++Fiterator) {
		// Look for call instructions
		for (inst_iterator Iiterator = inst_begin(*Fiterator); Iiterator != inst_end(*Fiterator); ++Iiterator){
			if (const CallInst *CI = dyn_cast<CallInst const>(&*Iiterator)){
				handleCallInst(PTA, &*Fiterator, CI);		
			}
		}
	}
}

int CallGraph::findNode(const Function* function) {
	for(auto& node : nodes) {
		if(node.getCaller() == function) {
			return node.getId();
		}
	}

	return -1;
}

void  CallGraph::BFS(const CGNode startNode, std::vector<bool> &visited) {	
	for(auto i : visited) {
		i = false;
	}
    
	std::list<int> queue;
	CGNode currentNode = startNode;
	visited[currentNode.getId()] = true;
	queue.push_back(currentNode.getId());
    
	while (!queue.empty()) {
		currentNode = nodes[queue.front()];
		queue.pop_front();
		for(auto nodeId : currentNode.calls) {
			if(!visited[nodeId]) {
				visited[nodeId] = true;
				queue.push_back(nodeId);
			}
		}
	}
}

bool CallGraph::containsCall(const Function* caller, const Function* callee) {
	CGNode callerNode = nodes[findNode(caller)];
	int calleeId = findNode(callee);
	std::vector<bool> visited(RESERVE_SIZE);
	BFS(callerNode, visited);
	return visited[calleeId];
}

bool CallGraph::containsDirectCall(const Function* caller, const Function* callee) {
	for(auto& node : nodes) {
		if(node.getCaller() == caller) {
			return node.containsCall(nodes, callee);
		}
	}
	return false;
}

bool CallGraph::isRecursive(const Function* function) {
	return containsCall(function, function);
}

void CallGraph::handleCallInst(std::unique_ptr<dg::LLVMPointerAnalysis> &PTA, const Function *F,  const CallInst *CI) {
	CGNode *caller = &nodes[findNode(F)];
	
	// Store called functions for F in its vector
	const Value *CV = CI->getCalledValue()->stripPointerCasts();
		
	if (const Function *calledF = dyn_cast<Function>(CV)) {
		if(!containsDirectCall(F, calledF)) {
			caller->calls.push_back(nodes[findNode(calledF)].getId());
		}
	} else {
		PSNode *psnode = PTA->getPointsTo(CV);
		for (auto& ptr : psnode->pointsTo) {
			Value *llvmValue = ptr.target->getUserData<llvm::Value>();
			if (const Function *calledF = dyn_cast<Function>(llvmValue)){
				if(!containsDirectCall(F, calledF)) {
					caller->calls.push_back(nodes[findNode(calledF)].getId());
				}
			}
		}
	}
}


