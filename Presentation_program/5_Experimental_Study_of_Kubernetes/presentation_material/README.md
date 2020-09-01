## About Work
This work was originally published in 4 August 2020 at APAN50 by 
- Mr Chanpol Kongsute
- Assoc. Prof. Dr Chaodit Aswakul

## The objective
we propose a pragmatic implementation framework in which Kubernetes clusters are federated in application-level as a peer-to-peer federation through Istio service mesh. This framework can be instantiated by configuring an Istio gateway to act as a cross-cluster load balancer handling traffic between clusters. This approach allows use cases such as cloud bursting and canary deployment. As a verification of this proposed federation framework, we use two Kubernetes clusters installed with echo servers. And by sending requests to the echo server locating in the cluster, we observe the behaviors and limitations of our proposed model.

## The Diagram of Proposed Medel (Cross-cluster communication)
Our design is based on the peer-to-peer federation model, where all management resources are distributed across all clusters, so there is no hierarchical relationship between clusters. In other words, each cluster will install its own Istio control plane. The only shared component is a common root certificate authority, which is needed for authentication in cross-cluster communication.
<h3 align="center"><img width="90%" src="/Presentation_program/5_Experimental_Study_of_Kubernetes/picture/diagram.png" /></h3>
