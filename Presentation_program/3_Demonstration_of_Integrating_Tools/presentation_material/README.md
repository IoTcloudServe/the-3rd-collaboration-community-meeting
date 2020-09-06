![Heading Collaboration](https://github.com/IoTcloudServe/the-3rd-collaboration-community-meeting/blob/master/Agenda/Heading.png)

# Intregating Rancher and Prometheus for Cluster Monitoring
Using Rancher, you can monitor the state and processes of your cluster nodes, Kubernetes components, and software deployments through integration with Prometheus, a leading open-source monitoring solution.

<p align="center">
  <img width="863" height="563" src="/Presentation_program/3_Demonstration_of_Integrating_Tools/picture/demo1.png"/>
</p>

## Monotoring Scope
You can monitor Rancher at both the cluster level and project level. For each cluster and project that is enabled for monitoring, Rancher deploys a Prometheus server.

* Cluster monitoring allows you to view the health of your Kubernetes cluster. Prometheus collects metrics from the cluster components below, which you can view in graphs and charts.
    
    * Kubernetes control plane
    * etcd database
    * All nodes (including workers)

* Project monitoring allows you to view the state of pods running in a given project. Prometheus collects metrics from the project’s deployed HTTP and TCP/UDP workloads.

# Intregating Rancher and Ceph Dashboard for Overview of Ceph Cluster
The dashboard is a very helpful tool to give you an overview of the status of your Ceph cluster, including overall health, status of the mon quorum, status of the mgr, osd, and other Ceph daemons, view pools and PG status, show logs for the daemons, and more. Rook makes it simple to enable the dashboard.

<p align="center">
  <img width="863" height="563" src="/Presentation_program/3_Demonstration_of_Integrating_Tools/picture/ceph-dashboard.png"/>
</p>

# Intregating Rancher and Longhorn Dashboard for Overview of Distributed Block Storage in Kubernetes Cluster

Longhorn implements distributed block storage using containers and microservices. Longhorn creates a dedicated storage controller for each block device volume and synchronously replicates the volume across multiple replicas stored on multiple nodes. The storage controller and replicas are themselves orchestrated using Kubernetes. Here are some notable features of Longhorn:

* Enterprise-grade distributed storage with no single point of failure
* Incremental snapshot of block storage
* Backup to secondary storage (NFSv4 or S3-compatible object storage) built on efficient change block detection
* Recurring snapshot and backup
* Automated non-disruptive upgrade. You can upgrade the entire Longhorn software stack without disrupting running volumes!
* Intuitive GUI dashboard


<p align="center">
  <img width="863" height="563" src="/Presentation_program/3_Demonstration_of_Integrating_Tools/picture/longhorn-ui.png"/>
</p>

