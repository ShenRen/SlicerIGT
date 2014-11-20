
// ToolWatchdog MRML includes
#include "vtkMRMLToolWatchdogNode.h"

// Other MRML includes
#include "vtkMRMLDisplayNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLTransformNode.h"

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkIntArray.h>
#include <vtkCommand.h>

// Other includes
#include <sstream>

// Constants
static const char* TOOL_ROLE = "ToolWatchdogTransformNode";



vtkMRMLToolWatchdogNode* vtkMRMLToolWatchdogNode
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLToolWatchdogNode" );
  if( ret )
    {
      return ( vtkMRMLToolWatchdogNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLToolWatchdogNode;
}



vtkMRMLToolWatchdogNode
::vtkMRMLToolWatchdogNode()
{
  this->HideFromEditorsOff();
  this->SetSaveWithScene( true );
  
  this->AddNodeReferenceRole( TOOL_ROLE );
}



vtkMRMLToolWatchdogNode
::~vtkMRMLToolWatchdogNode()
{
}



vtkMRMLNode*
vtkMRMLToolWatchdogNode
::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLToolWatchdogNode" );
  if( ret )
    {
      return ( vtkMRMLToolWatchdogNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLToolWatchdogNode;
}



void
vtkMRMLToolWatchdogNode
::WriteXML( ostream& of, int nIndent )
{
  Superclass::WriteXML(of, nIndent); // This will take care of referenced nodes
  vtkIndent indent(nIndent);
}



void
vtkMRMLToolWatchdogNode
::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes(atts); // This will take care of referenced nodes

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL)
  {
    attName  = *(atts++);
    attValue = *(atts++);
    
    if ( ! strcmp( attName, "WarningColorR" ) )
    {
      std::stringstream ss;
      ss << attValue;
      double r = 0.0;
      ss >> r;
    }
  }
}



void
vtkMRMLToolWatchdogNode
::Copy( vtkMRMLNode *anode )
{  
  Superclass::Copy( anode ); // This will take care of referenced nodes
  
  vtkMRMLToolWatchdogNode *node = ( vtkMRMLToolWatchdogNode* ) anode;
  
  
  this->Modified();
}



void
vtkMRMLToolWatchdogNode
::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkMRMLNode::PrintSelf(os,indent); // This will take care of referenced nodes

  os << indent << "ToolTipTransformID: " << this->GetTransformNode()->GetID() << std::endl;
}


vtkMRMLLinearTransformNode*
vtkMRMLToolWatchdogNode
::GetTransformNode()
{
  vtkMRMLLinearTransformNode* ltNode = vtkMRMLLinearTransformNode::SafeDownCast( this->GetNodeReference( TOOL_ROLE ) );
  return ltNode;
}


std::list<WatchedTransform> * 
vtkMRMLToolWatchdogNode
::GetTransformNodes()
{
  return &WatchedTransfroms;
}

void
vtkMRMLToolWatchdogNode
::AddTransformNode( vtkMRMLNode *mrmlNode)
{
  WatchedTransform tempWatchTransform;
  tempWatchTransform.transform=vtkMRMLLinearTransformNode::SafeDownCast(mrmlNode);
  WatchedTransfroms.push_back(tempWatchTransform);
}


void 
vtkMRMLToolWatchdogNode
::RemoveTransform(int row)
{
  std::list<WatchedTransform>::iterator it = WatchedTransfroms.begin();
  advance (it,row);
  WatchedTransfroms.erase(it);

  //int index=0;
  //for (std::list<WatchedTransform>::iterator it = WatchedTransfroms.begin() ; it != WatchedTransfroms.end(); ++it)
  //{
  //  QString transName((*it).transform->GetName());
  //  if(transName.compare(transformName)==0)
  //  {
  //    it = WatchedTransfroms.begin();
  //    WatchedTransfroms.erase(it+index);
  //    break;
  //  }
  //  index++;
  //}
}

void 
vtkMRMLToolWatchdogNode
::SwapMarkups( int trasformA, int trasformB )
{
  std::list<WatchedTransform>::iterator itA = WatchedTransfroms.begin();
  advance (itA,trasformA);
  std::list<WatchedTransform>::iterator itB = WatchedTransfroms.begin();
  advance (itB,trasformB);

  WatchedTransform transformTemp;
  transformTemp.status=itA->status;
  transformTemp.transform=itA->transform;
  transformTemp.LastTimeStamp=itA->LastTimeStamp;

  itA->status=itB->status;
  itA->transform=itB->transform;
  itA->LastTimeStamp=itB->LastTimeStamp;

  itB->status=transformTemp.status;
  itB->transform=transformTemp.transform;
  itB->LastTimeStamp=transformTemp.LastTimeStamp;
}



bool 
vtkMRMLToolWatchdogNode
::HasTransform(char * transformName)
{
  //QString transName(transformName);
  for (std::list<WatchedTransform>::iterator it = WatchedTransfroms.begin() ; it != WatchedTransfroms.end(); ++it)
  {
    QString transNameTemp((*it).transform->GetName());
    if(transNameTemp.compare(transformName)==0)
    {
      return true;
    }
  }
  return false;
}

int 
vtkMRMLToolWatchdogNode
::GetNumberOfTransforms()
{
  return WatchedTransfroms.size();
}

void
vtkMRMLToolWatchdogNode
::SetAndObserveToolTransformNodeId( const char* nodeId )
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue( vtkCommand::ModifiedEvent );
  events->InsertNextValue( vtkMRMLLinearTransformNode::TransformModifiedEvent );
  this->SetAndObserveNodeReferenceID( TOOL_ROLE, nodeId, events.GetPointer() );
}



void
vtkMRMLToolWatchdogNode
::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  vtkMRMLNode* callerNode = vtkMRMLNode::SafeDownCast( caller );
  if ( callerNode == NULL ) return;

  const char* ObservedTransformNodeId = this->GetTransformNode()->GetID();
  if ( strcmp( ObservedTransformNodeId, callerNode->GetID() ) == 0 )
  {
    this->Modified(); // This will tell the logic to update
  }
}
