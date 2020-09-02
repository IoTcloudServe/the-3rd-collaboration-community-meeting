![Heading Collaboration](https://github.com/IoTcloudServe/the-3rd-collaboration-community-meeting/blob/master/Agenda/Heading.png)

<h1>
<p align="center">
<strong> Demonstration of Rancher Resources Control policies </strong>
<p align="center">
</h1> 

<h3>
<p align="center">
<strong> How Resource Quota work on Rancher </strong>
<p align="center">
</h3> 
 
<p align="center">
  <img width="863" height="563" src="/Presentation_program/4_Demonstration_of_Rancher_Resource/presentation_material/Rancher_quota.PNG"/>
</p>

&nbsp;  In Rancher, you apply a resource quota to the project, and then the quota propagates to each namespace, whereafter Kubernetes enforces your limits using the native version of resource quotas. If you want to change the quota for a specific namespace, you can override it.

The resource quota includes two limits, which you set while creating or editing a project:

Project Limits:

This set of values configures an overall resource limit for the project. If you try to add a new namespace to the project, Rancher uses the limits you’ve set to validate that the project has enough resources to accommodate the namespace. In other words, if you try to move a namespace into a project near its resource quota, Rancher blocks you from moving the namespace.

Namespace Default Limits:

This value is the default resource limit available for each namespace. When the resource quota is created at the project level, this limit is automatically propagated to each namespace in the project. Each namespace is bound to this default limit unless you override it.

In the following diagram, a Rancher administrator wants to apply a resource quota that sets the same CPU and memory limit for every namespace in their project (Namespace 1-4). However, in Rancher, the administrator can set a resource quota for the project (Project Resource Quota) rather than individual namespaces. This quota includes resource limits for both the entire project (Project Limit) and individual namespaces (Namespace Default Limit). Rancher then propagates the Namespace Default Limit quotas to each namespace (Namespace Resource Quota) when created.

<h3>
<p align="center">
<strong> Resource Control Implementation in IoTCloudServe@TEIN  </strong>
</p>
</h3>
 
<p align="center">
  <img width="863" height="580" src="/Presentation_program/4_Demonstration_of_Rancher_Resource/presentation_material/Resource Control.PNG"/>
</p>

&nbsp; In our IOTCloudServer, There are 7 projects. Each project and namespace have their own quota limit (CPU, Mem, Pod, PVC).

list of projects in our IOTCloudServe@TEIN cluster

Smart Mobility 01 : SDWMN
Smart Mobility 02 : Traffic Analysis

Smart Agriculture 01 : NUOL Team

Smart Energy 01 : NETPIE based CUBEM gateway  
Smart Energy 02 : CU Smart Campus
Smart Energy 03 : CUBEM Data Analytic Parallelization
Smart Energy 04 : CU Smart Classroom

<h3>
<p align="center">
<strong> Resource Quota Type Reference </strong>
</p>
</h3>

<p align="center">
  <img width="863" height="570" src="/Presentation_program/4_Demonstration_of_Rancher_Resource/presentation_material/Parameters.PNG"/>
</p>

&nbsp; When you create a resource quota, you are configuring the pool of resources available to the project. You can set the following resource limits for the following resource types via Rancher GUI.

I our Project we considered 4 resource types i.e. CPU(cores, millicore), Mem(Gib, Mib), Pods,  


  
&nbsp; 

<h6><a href="https://202.28.193.103">>>Click Here for visit IoTCloudServe :)<<</a></h6>

</details>

<details>
    <summary>Concept Limit and Reserve</summary>
    <p align="center">
  <img width="300" height="280" src="/Presentation_program/4_Demonstration_of_Rancher_Resource/presentation_material/Lim&Req.PNG"/>
</p>
  
&nbsp; Requests and limits are the mechanisms Kubernetes uses to control resources such as CPU and memory. Requests are what the container is guaranteed to get. If a container requests a resource, Kubernetes will only schedule it on a node that can give it that resource. Limits, on the other hand, make sure a container never goes above a certain value. The container is only allowed to go up to the limit, and then it is restricted.

It is important to remember that the limit can never be lower than the request. If you try this, Kubernetes will throw an error and won’t let you run the container.

Requests and limits are on a per-container basis. While Pods usually contain a single container, it’s common to see Pods with multiple containers as well. Each container in the Pod gets its own individual limit and request, but because Pods are always scheduled as a group, you need to add the limits and requests for each container together to get an aggregate value for the Pod.

To control what requests and limits a container can have, you can set quotas at the Container level and at the Namespace level. If you want to learn more about Namespaces, see this previous installment from our blog series!

Let’s see how these work.
</details>


</details>

<details>
    <summary> Namespace quota setting </summary>
    <p align="center">
  <img width="300" height="300" src="/Presentation_program/4_Demonstration_of_Rancher_Resource/presentation_material/example.PNG"/>
</p>
  
&nbsp; 

From this Example 

there are four configuration in this yaml file 

requests.cpu is the maximum combined CPU requests in millicores for all the containers in the Namespace. In the above example, you can have 50 containers with 10m requests, five containers with 100m requests, or even one container with a 500m request. As long as the total requested CPU in the Namespace is less than 500m!

requests.memory is the maximum combined Memory requests for all the containers in the Namespace. In the above example, you can have 50 containers with 500MiB requests, five containers with 100MiB CPU requests, or even a single container with a 500MiB request. As long as the total requested Memory in the Namespace is less than 500MiB!

limits.cpu is the maximum combined CPU limits for all the containers in the Namespace. It’s just like requests.cpu but for the limit.

limits.memory is the maximum combined Memory limits for all containers in the Namespace. It’s just like requests.memory but for the limit.

If you are using a production and development Namespace (in contrast to a Namespace per team or service), a common pattern is to put no quota on the production Namespace and strict quotas on the development Namespace. This allows production to take all the resources it needs in case of a spike in traffic.

</details>


</details>

<details>
    <summary> Individual Container quota setting </summary>
    <p align="center">
  <img width="300" height="360" src="/Presentation_program/4_Demonstration_of_Rancher_Resource/presentation_material/LimRange.PNG"/> </p>
  
&nbsp; 

Unlike a Quota, which looks at the Namespace as a whole, a LimitRange applies to an individual container. This can help prevent people from creating super tiny or super large containers inside the Namespace.

Looking at this example, you can see there are four sections. Again, setting each of these sections is optional.

The default section sets up the default limits for a container in a pod. If you set these values in the limitRange, any containers that don’t explicitly set these themselves will get assigned the default values.

The defaultRequest section sets up the default requests for a container in a pod. If you set these values in the limitRange, any containers that don’t explicitly set these themselves will get assigned the default values.

The max section will set up the maximum limits that a container in a Pod can set. The default section cannot be higher than this value. Likewise, limits set on a container cannot be higher than this value. It is important to note that if this value is set and the default section is not, any containers that don’t explicitly set these values themselves will get assigned the max values as the limit.

The min section sets up the minimum Requests that a container in a Pod can set. The defaultRequest section cannot be lower than this value. Likewise, requests set on a container cannot be lower than this value either. It is important to note that if this value is set and the defaultRequest section is not, the min value becomes the defaultRequest value too.


</details>

<details>
    <summary> 1st Demo POD </summary>
      
&nbsp; In this Demo, I showed that I could set POD quota. 

I will configured quota resource in the namespace to limit the number of pod in the namespace 

the configured file (.ymal) is shown below : 

<p align="center">
  <img width="300" height="280" src="/Presentation_program/4_Demonstration_of_Rancher_Resource/presentation_material/pod.PNG"/> </p>

as you can see from .yaml Type : ResourceQuota (it means that we applied this quota to the namespace)

pod: "6" means "in this namespace, there should not be more than 6 pods" .


<h6><a href="https://202.28.193.103">>>Live Demo via IoTCloudServe :)<<</a></h6>



</details>

<details>
    <summary> 2nd Demo CPU Mem </summary>
      
&nbsp; This Demo shows how admins can limit&request cpu mem on the container level. we are going to show that by our quota configuration, even if containers consume cpu mem resources over the specified quota Limit, the user can not uses the resources more than limit.

Deploy Configuration file (.ymal) we deploy test workload for cpu and mem stress test and also set the specific quota.

<p align="center">
  <img width="300" height="280" src="/Presentation_program/4_Demonstration_of_Rancher_Resource/presentation_material/quota-mem-cpu.PNG"/> </p>

looking at this file we reserve the mem and cpu (600Mb and 400 millicores) and set the maximum quota to 800 Mb and 800 millicores" .


<h6><a href="https://202.28.193.103">>>Live Demo via IoTCloudServe :)<<</a></h6>



</details>


