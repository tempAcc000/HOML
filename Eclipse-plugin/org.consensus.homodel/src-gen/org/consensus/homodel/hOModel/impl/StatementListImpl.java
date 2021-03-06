/**
 * generated by Xtext 2.26.0
 */
package org.consensus.homodel.hOModel.impl;

import java.util.Collection;

import org.consensus.homodel.hOModel.HOModelPackage;
import org.consensus.homodel.hOModel.Invariant;
import org.consensus.homodel.hOModel.Statement;
import org.consensus.homodel.hOModel.StatementList;
import org.consensus.homodel.hOModel.TransitionPart;
import org.consensus.homodel.hOModel.Univalence;
import org.consensus.homodel.hOModel.Variable;

import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Statement List</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * </p>
 * <ul>
 *   <li>{@link org.consensus.homodel.hOModel.impl.StatementListImpl#getVar <em>Var</em>}</li>
 *   <li>{@link org.consensus.homodel.hOModel.impl.StatementListImpl#getExp <em>Exp</em>}</li>
 *   <li>{@link org.consensus.homodel.hOModel.impl.StatementListImpl#getStatement <em>Statement</em>}</li>
 *   <li>{@link org.consensus.homodel.hOModel.impl.StatementListImpl#getStatements <em>Statements</em>}</li>
 * </ul>
 *
 * @generated
 */
public class StatementListImpl extends DomainImpl implements StatementList
{
  /**
   * The cached value of the '{@link #getVar() <em>Var</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getVar()
   * @generated
   * @ordered
   */
  protected EList<Variable> var;

  /**
   * The cached value of the '{@link #getExp() <em>Exp</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getExp()
   * @generated
   * @ordered
   */
  protected EList<EObject> exp;

  /**
   * The cached value of the '{@link #getStatement() <em>Statement</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getStatement()
   * @generated
   * @ordered
   */
  protected EList<Statement> statement;

  /**
   * The cached value of the '{@link #getStatements() <em>Statements</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getStatements()
   * @generated
   * @ordered
   */
  protected EList<Statement> statements;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected StatementListImpl()
  {
    super();
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  protected EClass eStaticClass()
  {
    return HOModelPackage.Literals.STATEMENT_LIST;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public EList<Variable> getVar()
  {
    if (var == null)
    {
      var = new EObjectContainmentEList<Variable>(Variable.class, this, HOModelPackage.STATEMENT_LIST__VAR);
    }
    return var;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public EList<EObject> getExp()
  {
    if (exp == null)
    {
      exp = new EObjectContainmentEList<EObject>(EObject.class, this, HOModelPackage.STATEMENT_LIST__EXP);
    }
    return exp;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public EList<Statement> getStatement()
  {
    if (statement == null)
    {
      statement = new EObjectContainmentEList<Statement>(Statement.class, this, HOModelPackage.STATEMENT_LIST__STATEMENT);
    }
    return statement;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public EList<Statement> getStatements()
  {
    if (statements == null)
    {
      statements = new EObjectContainmentEList<Statement>(Statement.class, this, HOModelPackage.STATEMENT_LIST__STATEMENTS);
    }
    return statements;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs)
  {
    switch (featureID)
    {
      case HOModelPackage.STATEMENT_LIST__VAR:
        return ((InternalEList<?>)getVar()).basicRemove(otherEnd, msgs);
      case HOModelPackage.STATEMENT_LIST__EXP:
        return ((InternalEList<?>)getExp()).basicRemove(otherEnd, msgs);
      case HOModelPackage.STATEMENT_LIST__STATEMENT:
        return ((InternalEList<?>)getStatement()).basicRemove(otherEnd, msgs);
      case HOModelPackage.STATEMENT_LIST__STATEMENTS:
        return ((InternalEList<?>)getStatements()).basicRemove(otherEnd, msgs);
    }
    return super.eInverseRemove(otherEnd, featureID, msgs);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public Object eGet(int featureID, boolean resolve, boolean coreType)
  {
    switch (featureID)
    {
      case HOModelPackage.STATEMENT_LIST__VAR:
        return getVar();
      case HOModelPackage.STATEMENT_LIST__EXP:
        return getExp();
      case HOModelPackage.STATEMENT_LIST__STATEMENT:
        return getStatement();
      case HOModelPackage.STATEMENT_LIST__STATEMENTS:
        return getStatements();
    }
    return super.eGet(featureID, resolve, coreType);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @SuppressWarnings("unchecked")
  @Override
  public void eSet(int featureID, Object newValue)
  {
    switch (featureID)
    {
      case HOModelPackage.STATEMENT_LIST__VAR:
        getVar().clear();
        getVar().addAll((Collection<? extends Variable>)newValue);
        return;
      case HOModelPackage.STATEMENT_LIST__EXP:
        getExp().clear();
        getExp().addAll((Collection<? extends EObject>)newValue);
        return;
      case HOModelPackage.STATEMENT_LIST__STATEMENT:
        getStatement().clear();
        getStatement().addAll((Collection<? extends Statement>)newValue);
        return;
      case HOModelPackage.STATEMENT_LIST__STATEMENTS:
        getStatements().clear();
        getStatements().addAll((Collection<? extends Statement>)newValue);
        return;
    }
    super.eSet(featureID, newValue);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public void eUnset(int featureID)
  {
    switch (featureID)
    {
      case HOModelPackage.STATEMENT_LIST__VAR:
        getVar().clear();
        return;
      case HOModelPackage.STATEMENT_LIST__EXP:
        getExp().clear();
        return;
      case HOModelPackage.STATEMENT_LIST__STATEMENT:
        getStatement().clear();
        return;
      case HOModelPackage.STATEMENT_LIST__STATEMENTS:
        getStatements().clear();
        return;
    }
    super.eUnset(featureID);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public boolean eIsSet(int featureID)
  {
    switch (featureID)
    {
      case HOModelPackage.STATEMENT_LIST__VAR:
        return var != null && !var.isEmpty();
      case HOModelPackage.STATEMENT_LIST__EXP:
        return exp != null && !exp.isEmpty();
      case HOModelPackage.STATEMENT_LIST__STATEMENT:
        return statement != null && !statement.isEmpty();
      case HOModelPackage.STATEMENT_LIST__STATEMENTS:
        return statements != null && !statements.isEmpty();
    }
    return super.eIsSet(featureID);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public int eBaseStructuralFeatureID(int derivedFeatureID, Class<?> baseClass)
  {
    if (baseClass == TransitionPart.class)
    {
      switch (derivedFeatureID)
      {
        default: return -1;
      }
    }
    if (baseClass == Invariant.class)
    {
      switch (derivedFeatureID)
      {
        default: return -1;
      }
    }
    if (baseClass == Univalence.class)
    {
      switch (derivedFeatureID)
      {
        default: return -1;
      }
    }
    if (baseClass == Statement.class)
    {
      switch (derivedFeatureID)
      {
        case HOModelPackage.STATEMENT_LIST__VAR: return HOModelPackage.STATEMENT__VAR;
        case HOModelPackage.STATEMENT_LIST__EXP: return HOModelPackage.STATEMENT__EXP;
        case HOModelPackage.STATEMENT_LIST__STATEMENT: return HOModelPackage.STATEMENT__STATEMENT;
        default: return -1;
      }
    }
    return super.eBaseStructuralFeatureID(derivedFeatureID, baseClass);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public int eDerivedStructuralFeatureID(int baseFeatureID, Class<?> baseClass)
  {
    if (baseClass == TransitionPart.class)
    {
      switch (baseFeatureID)
      {
        default: return -1;
      }
    }
    if (baseClass == Invariant.class)
    {
      switch (baseFeatureID)
      {
        default: return -1;
      }
    }
    if (baseClass == Univalence.class)
    {
      switch (baseFeatureID)
      {
        default: return -1;
      }
    }
    if (baseClass == Statement.class)
    {
      switch (baseFeatureID)
      {
        case HOModelPackage.STATEMENT__VAR: return HOModelPackage.STATEMENT_LIST__VAR;
        case HOModelPackage.STATEMENT__EXP: return HOModelPackage.STATEMENT_LIST__EXP;
        case HOModelPackage.STATEMENT__STATEMENT: return HOModelPackage.STATEMENT_LIST__STATEMENT;
        default: return -1;
      }
    }
    return super.eDerivedStructuralFeatureID(baseFeatureID, baseClass);
  }

} //StatementListImpl
